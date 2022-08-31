#include "qual_encoder.h"

// -----------------------------------------------------------------------------

#include <algorithm>
#include <fstream>
#include <iostream>

// -----------------------------------------------------------------------------

#include "calq_coder.h"
#include "error_exception_reporter.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

QualEncoder::QualEncoder(const EncodingOptions& options, const std::map<int, Quantizer>& quant, DecodingBlock* o)
    : nrMappedRecords_(0),
      minPosUnencoded(0),
      NR_QUANTIZERS(options.quantizationMax - options.quantizationMin + 1),

      qualityValueOffset_(options.qualityValueOffset),
      posOffset_(0),
      samPileupDeque_(),
      recordPileup(),
      haplotyper_(options.filterSize, options.polyploidy, options.qualityValueOffset,
                  static_cast<size_t>(NR_QUANTIZERS), options.hqSoftClipPropagation, options.hqSoftClipStreak,
                  options.filterCutOff, options.debugPileup, options.squash, options.filterType),
      genotyper_(static_cast<const int&>(options.polyploidy), static_cast<const int&>(options.qualityValueOffset),
                 NR_QUANTIZERS, options.debugPileup),

      out(o),

      posCounter(0),
      hqSoftClipThreshold(options.hqSoftClipThreshold),
      quantizers_(quant),

      samRecordDeque_(),

      version_(options.version) {}

// -----------------------------------------------------------------------------

QualEncoder::~QualEncoder() = default;

// -----------------------------------------------------------------------------

void QualEncoder::addMappedRecordToBlock(const std::vector<std::string>& seqs, const std::vector<std::string>& qvalues,
                                         const std::vector<std::string>& cigars,
                                         const std::vector<std::uint32_t>& positions) {
    if (nrMappedRecords() == 0) {
        posOffset_ = positions.front();
        minPosUnencoded = positions.front();

        out->codeBooks.clear();
        out->stepindices.clear();
        for (int i = 0; i < NR_QUANTIZERS; ++i) {
            const auto& map = quantizers_[i].inverseLut();
            out->codeBooks.emplace_back();
            out->stepindices.emplace_back();
            for (const auto& pair : map) {
                out->codeBooks.back().push_back(static_cast<uint8_t>(pair.second));
            }
        }
        out->quantizerIndices.clear();
    }

    for (size_t i = 0; i < positions.size(); ++i) {
        recordPileup.addSingleRead(seqs[i], qvalues[i], cigars[i], positions[i]);
    }
    recordPileup.nextRecord();

    // calc quantizers
    while(minPosUnencoded < positions.front()){

        std::string pos_seqs, pos_qvalues;

        std::tie(pos_seqs, pos_qvalues) = recordPileup.getPileup(minPosUnencoded);

        if(!pos_seqs.empty()){
            auto l = uint8_t(genotyper_.computeQuantizerIndex(pos_seqs, pos_qvalues));
            out->quantizerIndices.push_back(l);
        }

        ++minPosUnencoded;
    }

    // encode qvalues
    recordPileup.getRecordsBefore()






    ++nrMappedRecords_;
}


// -----------------------------------------------------------------------------

void QualEncoder::finishBlock() {
    // Compute all remaining quantizers
    if (version_ == Version::V2) {
        while (!samPileupDeque_.empty()) {
            auto k = static_cast<uint8_t>(haplotyper_.push(samPileupDeque_.front().seq, samPileupDeque_.front().qual,
                                                           samPileupDeque_.front().hq_softcounter,
                                                           samPileupDeque_.front().ref));
            ++posCounter;
            if (posCounter > haplotyper_.getOffset()) {
                out->quantizerIndices.push_back(k);
            }
            samPileupDeque_.pop_front();
        }

        // Empty pipeline
        size_t offset = std::min(posCounter, haplotyper_.getOffset());
        for (size_t i = 0; i < offset; ++i) {
            uint8_t k = static_cast<uint8_t>(haplotyper_.push("", "", 0, 'N'));
            out->quantizerIndices.push_back(k);
        }
    } else {
        // Compute all remaining quantizers
        while (!samPileupDeque_.empty()) {
            auto k =
                uint8_t(genotyper_.computeQuantizerIndex(samPileupDeque_.front().seq, samPileupDeque_.front().qual));
            out->quantizerIndices.push_back(k);
            samPileupDeque_.pop_front();
        }
    }

    // TODO(muenteferi): borders of blocks probably too low activity

    // Process all remaining records from queue
    while (!samRecordDeque_.empty()) {
        encodeMappedQual(samRecordDeque_.front());
        samRecordDeque_.pop_front();
    }

    posCounter = 0;
}

// -----------------------------------------------------------------------------

size_t QualEncoder::nrMappedRecords() const { return nrMappedRecords_; }

// -----------------------------------------------------------------------------
void QualEncoder::encodeMappedQual(const std::string& qvalues, const std::string& cigar, const uint32_t pos) {
    size_t cigarIdx = 0;
    size_t cigarLen = cigar.length();
    size_t opLen = 0;  // length of current CIGAR operation
    size_t qualIdx = 0;
    size_t quantizerIndicesIdx = pos - posOffset_;

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(cigar[cigarIdx])) {
            opLen = opLen * 10 + (size_t)cigar[cigarIdx] - (size_t)'0';
            continue;
        }

        switch (cigar[cigarIdx]) {
            case 'A':
            case 'C':
            case 'G':
            case 'T':
            case 'N':
                opLen = 1;
                /* fall through */
            case '=':
                // Encode opLen quality values with computed quantizer indices
                for (size_t i = 0; i < opLen; i++) {
                    uint8_t q = uint8_t(qvalues[qualIdx++]) - qualityValueOffset_;
                    uint8_t quantizerIndex = out->quantizerIndices[quantizerIndicesIdx++];
                    uint8_t qualityValueIndex = uint8_t(quantizers_.at(quantizerIndex).valueToIndex(q));
                    out->stepindices.at(quantizerIndex).push_back(qualityValueIndex);
                }
                break;
            case '+':
            case ')':
                // Encode opLen quality values with max quantizer index
                for (size_t i = 0; i < opLen; i++) {
                    auto q = static_cast<uint8_t>(qvalues[qualIdx++]) - qualityValueOffset_;
                    uint8_t qualityValueIndex = uint8_t(quantizers_.at(NR_QUANTIZERS - 1).valueToIndex(q));
                    out->stepindices.at(static_cast<size_t>(NR_QUANTIZERS - 1)).push_back(qualityValueIndex);
                }
                break;
            case '-':
                quantizerIndicesIdx += opLen;
                break;  // do nothing as these bases are not present
            case ']':
                break;  // these have been clipped
            case '(':
            case '[':
                break;  // ignore first clip char
            default:
                throwErrorException("Bad CIGAR string");
        }
        opLen = 0;
    }
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

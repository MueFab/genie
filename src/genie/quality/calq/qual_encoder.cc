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

void QualEncoder::addMappedRecordToBlock(EncodingRecord& record) {
    if (nrMappedRecords() == 0) {
        posOffset_ = record.positions.front();
        minPosUnencoded = record.positions.front();

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

    recordPileup.addRecord(record);

    // calc quantizers
    while (minPosUnencoded < record.positions.front()) {
        std::string pos_seqs, pos_qvalues;

        std::tie(pos_seqs, pos_qvalues) = recordPileup.getPileup(minPosUnencoded);

        if (!pos_seqs.empty()) {
            auto l = uint8_t(genotyper_.computeQuantizerIndex(pos_seqs, pos_qvalues));
            out->quantizerIndices.push_back(l);
        }

        ++minPosUnencoded;
    }

    // encode qvalues
    auto recordsToEncode = recordPileup.getRecordsBefore(minPosUnencoded + 1);

    for (auto& r : recordsToEncode) {
        for (size_t i = 0; i < r.positions.size(); ++i) {
            encodeMappedQual(r.qvalues[i], r.cigars[i], r.positions[i]);
        }
    }

    ++nrMappedRecords_;
}

// -----------------------------------------------------------------------------

void QualEncoder::finishBlock() {
    // Compute all remaining quantizers
    while (minPosUnencoded <= recordPileup.getMaxPos()) {
        std::string pos_seqs, pos_qvalues;
        std::tie(pos_seqs, pos_qvalues) = recordPileup.getPileup(minPosUnencoded);

        if (!pos_seqs.empty()) {
            auto l = uint8_t(genotyper_.computeQuantizerIndex(pos_seqs, pos_qvalues));
            out->quantizerIndices.push_back(l);
        }
        ++minPosUnencoded;
    }

    // Encode remaining records
    auto records = recordPileup.getAllRecords();
    for (auto& r : records) {
        for (size_t i = 0; i < r.positions.size(); ++i) {
            encodeMappedQual(r.qvalues[i], r.cigars[i], r.positions[i]);
        }
    }
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

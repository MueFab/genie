#include "calq/qual_decoder.h"

// -----------------------------------------------------------------------------

#include "calq/calq_coder.h"
#include "calq/error_exception_reporter.h"
#include "calq/qual_encoder.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

QualDecoder::QualDecoder(const DecodingBlock& b,
                         uint32_t positionOffset,
                         uint8_t qualityOffset,
                         EncodingBlock *o
)
        : posOffset_(positionOffset),
        qualityValueOffset_(qualityOffset),
        uqvIdx_(0),
        qviIdx_(b.stepindices.size(), 0),
        quantizers_(0),
        out(o),
        in(b){
    out->qvalues.clear();
    for (const auto& q : b.codeBooks) {
        std::map<int, int> steps;
        for (unsigned int i = 0; i < q.size(); ++i) {
            steps[i] = q[i];
        }
        quantizers_.emplace_back(steps);
    }
}

// -----------------------------------------------------------------------------

QualDecoder::~QualDecoder() = default;

// -----------------------------------------------------------------------------

void QualDecoder::decodeMappedRecordFromBlock(const DecodingRead& samRecord){
    std::string qual;

    size_t cigarIdx = 0;
    size_t cigarLen = samRecord.cigar.length();
    size_t opLen = 0;
    size_t qvciPos = samRecord.posMin - posOffset_;

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(samRecord.cigar[cigarIdx])) {
            opLen = opLen * 10 + (size_t) samRecord.cigar[cigarIdx]
                    - (size_t) '0';
            continue;
        }

        switch (samRecord.cigar[cigarIdx]) {
            case 'M':
            case '=':
            case 'X':
                // Decode opLen quality value indices with computed
                // quantizer indices
                for (size_t i = 0; i < opLen; i++) {
                    uint8_t quantizerIndex =
                            in.quantizerIndices[qvciPos++] - '0';


                    uint8_t qualityValueIndex =
                            in.stepindices.at(
                                    static_cast<size_t>(quantizerIndex)
                            )[qviIdx_[quantizerIndex]++] - '0';


                    uint8_t q =
                            uint8_t(
                                    quantizers_.at(quantizerIndex)
                                            .indexToReconstructionValue(
                                                    qualityValueIndex
                                            )
                            );

                    qual += static_cast<char>(q + qualityValueOffset_);
                }
                break;
            case 'I':
            case 'S':
                // Decode opLen quality values with max quantizer index
                for (size_t i = 0; i < opLen; i++) {
                    int qualityValueIndex =
                            in.stepindices.at(quantizers_.size() - 1)
                            [qviIdx_[quantizers_.size() - 1]++] - '0';
                    int q = quantizers_.at(quantizers_.size() - 1)
                            .indexToReconstructionValue(qualityValueIndex);
                    qual += static_cast<char>(q + qualityValueOffset_);
                }
                break;
            case 'D':
            case 'N':
                qvciPos += opLen;
                break;  // do nothing as these bases are not present
            case 'H':
            case 'P':
                break;  // these have been clipped
            default:
                throwErrorException("Bad CIGAR string");
        }
        opLen = 0;
    }
    out->qvalues.push_back(std::move(qual));
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

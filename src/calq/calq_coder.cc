#include "calq/calq_coder.h"

// -----------------------------------------------------------------------------

#include <map>
#include <utility>

// -----------------------------------------------------------------------------

#include "calq/error_exception_reporter.h"
#include "calq/lloyd_max_quantizer.h"
#include "calq/qual_decoder.h"
#include "calq/qual_encoder.h"
#include "calq/uniform_min_max_quantizer.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------
// TODO: look here
static uint32_t computeLength(const std::string& cigar) {
    // Compute 0-based first position and 0-based last position this record
    // is mapped to on the reference used for alignment
    uint32_t posMax = 0;

    size_t cigarIdx = 0;
    size_t cigarLen = cigar.length();
    uint32_t opLen = 0;  // length of current CIGAR operation

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(cigar[cigarIdx])) {
            opLen = opLen * 10 + (uint32_t)cigar[cigarIdx] - (uint32_t)'0';
            continue;
        }
        switch (cigar[cigarIdx]) {
            case '=':
                posMax += opLen;
                break;
            case '+':
            case ')':
                break;
            case '-':
                posMax += opLen;
                break;
            case ']':
                break;  // these have been clipped
            case '(':
            case '[':
                break;  // ignore first char of clips
            case 'A':
            case 'C':
            case 'G':
            case 'T':
            case 'N':
                ++posMax;
                break;
            default:
                throwErrorException("Bad CIGAR string");
        }
        opLen = 0;
    }
    return posMax;
}

// -----------------------------------------------------------------------------

void encode(const EncodingOptions& opt, const SideInformation& sideInformation, const EncodingBlock& input,
            DecodingBlock* output) {
    ProbabilityDistribution pdf(opt.qualityValueMin, opt.qualityValueMax);

    // Check quality value range
    for (auto const& samRecord : input.qvalues) {
        for (auto const& q : samRecord) {
            if ((static_cast<int>(q) - opt.qualityValueOffset) < opt.qualityValueMin) {
                throwErrorException("Quality value too small");
            }
            if ((static_cast<int>(q) - opt.qualityValueOffset) > opt.qualityValueMax) {
                throwErrorException("Quality value too large");
            }
            pdf.addToPdf((static_cast<size_t>(q) - opt.qualityValueOffset));
        }
    }

    std::map<int, Quantizer> quantizers;

    for (auto i = static_cast<int>(opt.quantizationMin); i <= static_cast<int>(opt.quantizationMax); ++i) {
        if (opt.quantizerType == QuantizerType::UNIFORM) {
            UniformMinMaxQuantizer quantizer(static_cast<const int&>(opt.qualityValueMin),
                                             static_cast<const int&>(opt.qualityValueMax), i);
            quantizers.insert(std::pair<int, Quantizer>(static_cast<const int&>(i - opt.quantizationMin), quantizer));
        } else if (opt.quantizerType == QuantizerType::LLOYD_MAX) {
            LloydMaxQuantizer quantizer(static_cast<size_t>(i));
            quantizer.build(pdf);
            quantizers.insert(std::pair<int, Quantizer>(static_cast<const int&>(i - opt.quantizationMin), quantizer));
        } else {
            throwErrorException("Quantization Type not supported");
        }
    }

    // Encode the quality values
    QualEncoder qualEncoder(opt, quantizers, output);
    for (size_t i = 0; i < sideInformation.positions.size(); ++i) {
        std::string ref;
        uint32_t len = computeLength(sideInformation.cigars[i]);
        if (opt.version == Version::V2) {
            ref = sideInformation.reference.substr(sideInformation.positions[i] - sideInformation.positions[0], len);
        }
        EncodingRead r = {sideInformation.positions[i], sideInformation.positions[i] + len, input.qvalues[i],
                          sideInformation.cigars[i],    sideInformation.sequences[i],       ref};
        qualEncoder.addMappedRecordToBlock(r);
    }

    qualEncoder.finishBlock();
}

// -----------------------------------------------------------------------------

void decode(const DecodingOptions&, const SideInformation& sideInformation, const DecodingBlock& input,
            EncodingBlock* output) {
    // Decode the quality values
    QualDecoder qualDecoder(input, sideInformation.positions[0], sideInformation.qualOffset, output);
    output->qvalues.clear();
    for (size_t i = 0; i < sideInformation.positions.size(); ++i) {
        DecodingRead r = {sideInformation.positions[i], sideInformation.cigars[i]};
        qualDecoder.decodeMappedRecordFromBlock(r);
    }
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

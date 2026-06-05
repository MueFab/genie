/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/calq_coder.h"

// -----------------------------------------------------------------------------

#include <map>
#include <utility>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/error_exception_reporter.h"
#include "genie/quality/calq/lloyd_max_quantizer.h"
#include "genie/quality/calq/qual_decoder.h"
#include "genie/quality/calq/qual_encoder.h"
#include "genie/quality/calq/uniform_min_max_quantizer.h"

// -----------------------------------------------------------------------------
namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

void encode(const EncodingOptions& opt, const SideInformation& sideInformation, const EncodingBlock& input,
            DecodingBlock* output) {
    ProbabilityDistribution pdf(opt.qualityValueMin, opt.qualityValueMax);

    // Check quality value range
    for (auto const& samRecord : input.qvalues) {
        for (auto const& read : samRecord) {
            for (auto const& q : read) {
                if ((static_cast<int>(q) - opt.qualityValueOffset) < opt.qualityValueMin) {
                    throwErrorException("Quality value too small");
                }
                if ((static_cast<int>(q) - opt.qualityValueOffset) > opt.qualityValueMax) {
                    throwErrorException("Quality value too large");
                }
                pdf.addToPdf((static_cast<size_t>(q) - opt.qualityValueOffset));
            }
        }
    }

    std::map<int, Quantizer> quantizers;

    for (auto idx_i = static_cast<int>(opt.quantizationMin); idx_i <= static_cast<int>(opt.quantizationMax); ++idx_i) {
        if (opt.quantizerType == QuantizerType::UNIFORM) {
            UniformMinMaxQuantizer quantizer(static_cast<const int&>(opt.qualityValueMin),
                                             static_cast<const int&>(opt.qualityValueMax), idx_i);
            quantizers.insert(std::pair<int, Quantizer>(static_cast<const int&>(idx_i - opt.quantizationMin), quantizer));
        } else if (opt.quantizerType == QuantizerType::LLOYD_MAX) {
            LloydMaxQuantizer quantizer(static_cast<size_t>(idx_i));
            quantizer.build(pdf);
            quantizers.insert(std::pair<int, Quantizer>(static_cast<const int&>(idx_i - opt.quantizationMin), quantizer));
        } else {
            throwErrorException("Quantization Type not supported");
        }
    }

    // Encode the quality values
    QualEncoder qualEncoder(opt, quantizers, output);

    for (size_t idx_i = 0; idx_i < sideInformation.positions.size(); ++idx_i) {
        EncodingRecord record = {input.qvalues[idx_i], sideInformation.sequences[idx_i], sideInformation.cigars[idx_i],
                                 sideInformation.positions[idx_i]};
        qualEncoder.addMappedRecordToBlock(record);
    }

    qualEncoder.finishBlock();
}

// -----------------------------------------------------------------------------

void decode(const DecodingOptions&, const SideInformation& sideInformation, const DecodingBlock& input,
            EncodingBlock* output) {
    // Decode the quality values
    QualDecoder qualDecoder(input, sideInformation.positions[0][0], sideInformation.qualOffset, output);
    output->qvalues.clear();
    output->qvalues.emplace_back();
    for (size_t idx_i = 0; idx_i < sideInformation.positions[0].size(); ++idx_i) {
        DecodingRead r = {sideInformation.positions[0][idx_i], sideInformation.cigars[0][idx_i]};
        qualDecoder.decodeMappedRecordFromBlock(r);
    }
}

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_ENCODER_H_
#define SRC_GENIE_QUALITY_CALQ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-encoder.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/calq/uniform_min_max_quantizer.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/stringview.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

/**
 * @brief
 */
class Encoder : public core::QVEncoder {
 private:
    void fillCalqStructures(const core::record::Chunk& chunk, calq::EncodingOptions& opt,
                            calq::SideInformation& sideInformation, calq::EncodingBlock& input);
    void encodeAligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                       core::AccessUnit::Descriptor& desc);
    void addQualities(const core::record::Segment& s, core::AccessUnit::Descriptor& desc,
                      calq::UniformMinMaxQuantizer& quantizer);
    void encodeUnaligned(const core::record::Chunk& chunk, paramqv1::QualityValues1& param,
                         core::AccessUnit::Descriptor& desc);

 public:
    /**
     * @brief
     * @param rec
     * @return
     */
    core::QVEncoder::QVCoded process(const core::record::Chunk& rec) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

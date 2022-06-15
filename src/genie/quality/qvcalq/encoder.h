/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_QVCALQ_ENCODER_H_
#define SRC_GENIE_QUALITY_QVCALQ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "calq/calq_coder.h"
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-encoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/stringview.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

/**
 * @brief
 */
class Encoder : public core::QVEncoder {
 private:
//    calq::SideInformation getSideInformation(const core::record::Chunk& record);
//    calq::EncodingBlock getEncodingBlock(const core::record::Chunk& record);

    void encodeAligned(const core::record::Chunk& chunk); // , core::AccessUnit::Descriptor& desc);


 public:
    /**
     * @brief
     * @param rec
     * @return
     */
    core::QVEncoder::QVCoded process(const core::record::Chunk& rec) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVCALQ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
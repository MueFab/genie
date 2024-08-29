/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_
#define SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-encoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/stringview.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

/**
 * @brief
 */
class Encoder : public core::QVEncoder {
 private:
    /**
     * @brief
     * @param rec
     * @param param
     * @param desc
     */
    static void setUpParameters(const core::record::Chunk& rec, paramqv1::QualityValues1& param,
                                core::AccessUnit::Descriptor& desc);

    /**
     * @brief
     * @param s
     * @param ecigar
     * @param desc
     */
    static void encodeAlignedSegment(const core::record::Segment& s, const std::string& ecigar,
                                     core::AccessUnit::Descriptor& desc);

    /**
     * @brief
     * @param s
     * @param desc
     */
    static void encodeUnalignedSegment(const core::record::Segment& s, core::AccessUnit::Descriptor& desc);

 public:
    /**
     * @brief
     * @param rec
     * @return
     */
    core::QVEncoder::QVCoded process(const core::record::Chunk& rec) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

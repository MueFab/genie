/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_QVWRITEOUT_DECODER_H_
#define SRC_GENIE_QUALITY_QVWRITEOUT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-decoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvwriteout {

/**
 * @brief
 */
class Decoder : public core::QVDecoder {
 public:
    /**
     * @brief
     * @param param
     * @param ecigar_vec
     * @param desc
     * @return
     */
    std::tuple<std::vector<std::string>, core::stats::PerfStats> process(const core::parameter::QualityValues& param,
                                                                         const std::vector<std::string>& ecigar_vec,
                                                                         core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVWRITEOUT_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

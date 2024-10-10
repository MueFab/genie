/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_DECODER_H_
#define SRC_GENIE_QUALITY_CALQ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/qv-decoder.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief
 */
class Decoder : public core::QVDecoder {
 private:
    static bool isAligned(const core::AccessUnit::Descriptor& desc);
    static std::vector<std::string> decodeAligned(const quality::paramqv1::QualityValues1& param,
                                                  const std::vector<std::string>& ecigar_vec,
                                                  const std::vector<uint64_t>& positions,
                                                  core::AccessUnit::Descriptor& desc);
    static std::vector<std::string> decodeUnaligned(const quality::paramqv1::QualityValues1& param,
                                                    const std::vector<std::string>& ecigar_vec,
                                                    core::AccessUnit::Descriptor& desc);
    static void fillInput(calq::DecodingBlock& input, core::AccessUnit::Descriptor& desc,
                          const quality::paramqv1::QualityValues1& param);

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
                                                                         const std::vector<uint64_t>& positions,
                                                                         core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

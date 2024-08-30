/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_DECODER_H_
#define SRC_GENIE_ENTROPY_BSC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/entropy-decoder.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Module to decompress a BlockPayload back into a raw access unit using Gabac
 */
class Decoder : public core::EntropyDecoder {
 public:
    /**
     * @brief
     * @param param
     * @param d
     * @param mmCoderEnabled
     * @return
     */
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> process(
        const core::parameter::DescriptorSubseqCfg& param, core::AccessUnit::Descriptor& d,
        bool mmCoderEnabled) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

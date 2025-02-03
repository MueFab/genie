/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODER_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/entropy-decoder.h"
#include "genie/entropy/gabac/gabac-seq-conf-set.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Module to decompress a BlockPayload back into a raw access unit using Gabac
 */
class Decoder : public core::EntropyDecoder {
 private:
    /**
     * @brief Execute gabac library
     * @param conf Gabac configuration to use
     * @param in subsequence payload
     * @param mmCoderEnabled if mismatch coder is enabled
     */
    static core::AccessUnit::Subsequence decompress(const gabac::EncodingConfiguration& conf,
                                                    core::AccessUnit::Subsequence&& in, bool mmCoderEnabled);

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

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

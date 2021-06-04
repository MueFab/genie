/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ENTROPY_DECODER_H_
#define SRC_GENIE_CORE_ENTROPY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include "genie/core/access-unit.h"
#include "genie/core/module.h"
#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy decoders. They convert access unit payloads to raw access units
 */
class EntropyDecoder {
 public:
    /**
     * @brief
     */
    virtual ~EntropyDecoder() = default;

    /**
     * @brief
     * @param param
     * @param desc
     * @param mmCoderEnabled
     * @return
     */
    virtual std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> process(
        const parameter::DescriptorSubseqCfg& param, core::AccessUnit::Descriptor& desc, bool mmCoderEnabled) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ENTROPY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

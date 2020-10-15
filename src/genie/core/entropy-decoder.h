/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_DECODER_H
#define GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/descriptor_present/decoder.h>
#include "access-unit.h"
#include "module.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy decoders. They convert access unit payloads to raw access units
 */
class EntropyDecoder {
   public:
    /**
     *
     */
    virtual ~EntropyDecoder() = default;

    /**
     *
     * @param param
     * @param desc
     * @return
     */
    virtual std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> process(
        const parameter::DescriptorSubseqCfg& param, core::AccessUnit::Descriptor& desc, bool mmCoderEnabled) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
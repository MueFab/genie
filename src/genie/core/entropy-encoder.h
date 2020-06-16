/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_ENCODER_H
#define GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/descriptor_present/decoder.h>
#include "access-unit.h"
#include "module.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy coders. They convert raw access units to access unit payloads
 */
class EntropyEncoder {
   public:
    using EntropyCoded = std::tuple<parameter::DescriptorSubseqCfg, AccessUnit::Descriptor, core::stats::PerfStats>;  //!<

    /**
     *
     */
    ~EntropyEncoder() = default;

    /**
     *
     * @param rec
     * @return
     */
    virtual EntropyCoded process(core::AccessUnit::Descriptor& desc) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ENTROPY_ENCODER_H_
#define SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include "genie/core/access-unit.h"
#include "genie/core/module.h"
#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Interface for entropy coders. They convert raw access units to access unit payloads
 */
class EntropyEncoder {
 public:
    using EntropyCoded =
        std::tuple<parameter::DescriptorSubseqCfg, AccessUnit::Descriptor, core::stats::PerfStats>;  //!< @brief

    /**
     * @brief
     */
    virtual ~EntropyEncoder() = default;

    /**
     * @brief
     * @param desc
     * @return
     */
    virtual EntropyCoded process(core::AccessUnit::Descriptor& desc) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

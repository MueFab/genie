/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ENTROPY_ENCODER_H_
#define SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include "genie/core/access-unit.h"
#include "genie/core/module.h"
#include "genie/core/parameter/descriptor_present/decoder.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

class EntropyEncoderAnnotation {
 public:
    virtual ~EntropyEncoderAnnotation() = default;
    virtual core::record::annotation_access_unit::Record process(
        core::record::annotation_parameter_set::DescriptorConfiguration& desc) = 0;
};

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

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

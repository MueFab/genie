/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_ENCODER_H
#define GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <core/record/record.h>
#include <util/drain.h>
#include <util/source.h>
#include "access-unit-raw.h"

#include <utility>

namespace genie {
namespace core {


/**
 * @brief The basic interface for modules encoding quality values
 */
class QVEncoder{
   public:
    using QVCoded = std::pair<std::unique_ptr<core::parameter::QualityValues>, core::AccessUnitRaw::Descriptor>;
    virtual QVCoded encode(const record::Chunk& rec) = 0;

    /**
     * @Brief For polymorphic destruction
     */
    ~QVEncoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
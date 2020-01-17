/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_ENCODER_H
#define GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "access-unit-raw.h"

#include <genie/core/access-unit-payload.h>
#include <memory>

#include <genie/core/access-unit-raw.h>
#include <genie/util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

class EntropyEncoder : public util::Source<AccessUnitPayload>, public util::Drain<AccessUnitRaw> {
   public:
    void flowIn(AccessUnitRaw&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~EntropyEncoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
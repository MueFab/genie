/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_DECODER_H
#define GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "access-unit-raw.h"

#include <genie/core/access-unit-payload.h>
#include <genie/util/source.h>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

class EntropyDecoder : public util::Drain<AccessUnitPayload>, public util::Source<AccessUnitRaw> {
   public:
    void flowIn(AccessUnitPayload&& t, size_t id) override = 0;
    void dryIn() override = 0;
    ~EntropyDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
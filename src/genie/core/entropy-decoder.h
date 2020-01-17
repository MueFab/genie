/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_DECODER_H
#define GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/source.h>
#include <memory>
#include "access-unit-payload.h"
#include "access-unit-raw.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy decoders. They convert access unit payloads to raw access units
 */
class EntropyDecoder : public util::Drain<AccessUnitPayload>, public util::Source<AccessUnitRaw> {
   public:
    ~EntropyDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
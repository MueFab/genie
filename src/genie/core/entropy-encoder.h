/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_ENCODER_H
#define GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "access-unit-payload.h"
#include "access-unit-raw.h"
#include "module.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy coders. They convert raw access units to access unit payloads
 */
class EntropyEncoder : public Module<AccessUnitRaw, AccessUnitPayload> {
   public:
    /**
     *
     */
    ~EntropyEncoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
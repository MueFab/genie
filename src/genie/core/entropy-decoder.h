/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_DECODER_H
#define GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "access-unit.h"
#include "access-unit.h"
#include "module.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy decoders. They convert access unit payloads to raw access units
 */
class EntropyDecoder : public Module<AccessUnit, AccessUnit> {
   public:
    /**
     *
     */
    ~EntropyDecoder() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
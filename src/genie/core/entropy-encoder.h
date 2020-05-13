/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_ENCODER_H
#define GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/source.h>
#include <memory>
#include "access-unit-payload.h"
#include "access-unit-raw.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for entropy coders. They convert raw access units to access unit payloads
 */
class EntropyEncoder : public util::Source<AccessUnitPayload>, public util::Drain<AccessUnitRaw> {
   public:
    ~EntropyEncoder() override = default;
    void skipIn(const genie::util::Section&) override {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
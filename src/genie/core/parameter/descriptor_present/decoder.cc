/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/descriptor_present/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::write(util::BitWriter& writer) const { writer.write(encoding_mode_ID, 8); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Decoder::getMode() const { return encoding_mode_ID; }

// ---------------------------------------------------------------------------------------------------------------------

Decoder::Decoder(uint8_t _encoding_mode_id) : encoding_mode_ID(_encoding_mode_id) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Decoder::equals(const Decoder* dec) const { return encoding_mode_ID == dec->encoding_mode_ID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_parameterset/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameterset {

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::operator==(const USignature& other) const { return u_signature_length == other.u_signature_length; }

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature() : u_signature_length(std::nullopt) {}

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature(genie::util::BitReader& reader) {
    if (reader.read<bool>(1)) {
        u_signature_length = reader.read<uint8_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::isConstantLength() const { return u_signature_length != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t USignature::getConstLength() const { return *u_signature_length; }

// ---------------------------------------------------------------------------------------------------------------------

void USignature::setConstLength(uint8_t length) { u_signature_length = length; }

// ---------------------------------------------------------------------------------------------------------------------

void USignature::write(genie::util::BitWriter& writer) const {
    writer.write(u_signature_length != std::nullopt, 1);
    if (u_signature_length != std::nullopt) {
        writer.write(*u_signature_length, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameterset

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

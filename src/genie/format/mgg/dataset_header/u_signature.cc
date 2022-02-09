/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::operator==(const USignature& other) const { return const_length == other.const_length; }

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature() : const_length(boost::none) {}

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature(uint8_t _const_length) : const_length(_const_length) {}

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature(genie::util::BitReader& reader) {
    bool U_signature_constant_length = reader.read<bool>(1);
    if (U_signature_constant_length) {
        const_length = reader.read<uint8_t>(8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void USignature::write(genie::util::BitWriter& writer) const {
    writer.write(isConstLength(), 1);
    if (isConstLength()) {
        writer.write(getConstLength(), 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::isConstLength() const { return const_length != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t USignature::getConstLength() const { return *const_length; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

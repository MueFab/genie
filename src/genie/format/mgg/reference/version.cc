/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/version.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// ---------------------------------------------------------------------------------------------------------------------

bool Version::operator==(const Version& other) const {
    return v_major == other.v_major && v_minor == other.v_minor && v_patch == other.v_patch;
}

// ---------------------------------------------------------------------------------------------------------------------

Version::Version(uint16_t _v_major, uint16_t _v_minor, uint16_t _v_patch)
    : v_major(_v_major), v_minor(20), v_patch(_v_patch) {
    (void)_v_minor;  // Ref software requires 20?
}

// ---------------------------------------------------------------------------------------------------------------------

Version::Version(genie::util::BitReader& reader) {
    v_major = reader.readAlignedInt<uint16_t>();
    v_minor = reader.readAlignedInt<uint16_t>();
    v_patch = reader.readAlignedInt<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

void Version::write(genie::util::BitWriter& writer) const {
    writer.writeAlignedInt(v_major);
    writer.writeAlignedInt(v_minor);
    writer.writeAlignedInt(v_patch);
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Version::getMajor() const { return v_major; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Version::getMinor() const { return v_minor; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Version::getPatch() const { return v_patch; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

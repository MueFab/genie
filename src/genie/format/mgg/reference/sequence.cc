/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/sequence.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// ---------------------------------------------------------------------------------------------------------------------

bool Sequence::operator==(const Sequence& other) const {
    return name == other.name && sequence_length == other.sequence_length && sequence_id == other.sequence_id &&
           version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(std::string _name, const uint32_t length, const uint16_t id, const core::MPEGMinorVersion _version)
    : name(std::move(_name)), sequence_length(length), sequence_id(id), version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Sequence::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Sequence::getLength() const { return sequence_length; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Sequence::getID() const { return sequence_id; }

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(util::BitReader& reader, const core::MPEGMinorVersion _version) : version(_version) {
    name = reader.readAlignedStringTerminated();
    if (version != core::MPEGMinorVersion::V1900) {
        sequence_length = reader.readAlignedInt<uint32_t>();
        sequence_id = reader.readAlignedInt<uint16_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Sequence::write(util::BitWriter& writer) const {
    writer.writeAlignedBytes(name.data(), name.length());
    writer.writeAlignedInt<uint8_t>('\0');
    if (version != core::MPEGMinorVersion::V1900) {
        writer.writeAlignedInt(sequence_length);
        writer.writeAlignedInt(sequence_id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(core::meta::Sequence s, const core::MPEGMinorVersion _version)
    : name(std::move(s.getName())),
      sequence_length(static_cast<uint32_t>(s.getLength())),
      sequence_id(s.getID()),
      version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/sequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {

// ---------------------------------------------------------------------------------------------------------------------

bool Sequence::operator==(const Sequence& other) const {
    return name == other.name && sequence_length == other.sequence_length && sequence_id == other.sequence_id &&
           version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(std::string _name, uint32_t length, uint16_t id, genie::core::MPEGMinorVersion _version)
    : name(std::move(_name)), sequence_length(length), sequence_id(id), version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Sequence::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Sequence::getLength() const { return sequence_length; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Sequence::getID() const { return sequence_id; }

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(genie::util::BitReader& reader, genie::core::MPEGMinorVersion _version) : version(_version) {
    reader.readBypass_null_terminated(name);
    if (version != genie::core::MPEGMinorVersion::V1900) {
        sequence_length = reader.readBypassBE<uint32_t>();
        sequence_id = reader.readBypassBE<uint16_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Sequence::write(genie::util::BitWriter& writer) const {
    writer.writeBypass(name.data(), name.length());
    writer.writeBypassBE<uint8_t>('\0');
    if (version != genie::core::MPEGMinorVersion::V1900) {
        writer.writeBypassBE(sequence_length);
        writer.writeBypassBE(sequence_id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Sequence::Sequence(genie::core::meta::Sequence s, genie::core::MPEGMinorVersion _version)
    : name(std::move(s.getName())),
      sequence_length(static_cast<uint32_t>(s.getLength())),
      sequence_id(s.getID()),
      version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool Reference::ReferenceSeq::operator==(const ReferenceSeq& other) const {
    return name == other.name && sequence_length == other.sequence_length && sequence_id == other.sequence_id &&
           version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceSeq::ReferenceSeq(std::string _name, uint32_t length, uint16_t id,
                                      genie::core::MPEGMinorVersion _version)
    : name(std::move(_name)), sequence_length(length), sequence_id(id), version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::ReferenceSeq::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Reference::ReferenceSeq::getLength() const { return sequence_length; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceSeq::getID() const { return sequence_id; }

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceSeq::ReferenceSeq(genie::util::BitReader& reader, genie::core::MPEGMinorVersion _version)
    : version(_version) {
    reader.readBypass_null_terminated(name);
    if (version != genie::core::MPEGMinorVersion::V1900) {
        sequence_length = reader.readBypassBE<uint32_t>();
        sequence_id = reader.readBypassBE<uint16_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::ReferenceSeq::write(genie::util::BitWriter& writer) const {
    writer.writeBypass(name.data(), name.length());
    writer.writeBypassBE<uint8_t>('\0');
    if (version != genie::core::MPEGMinorVersion::V1900) {
        writer.writeBypassBE(sequence_length);
        writer.writeBypassBE(sequence_id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reference::ReferenceVersion::operator==(const ReferenceVersion& other) const {
    return major == other.major && minor == other.minor && patch == other.patch;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceVersion::ReferenceVersion(uint16_t _major, uint16_t _minor, uint16_t _patch)
    : major(_major), minor(_minor), patch(_patch) {}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceVersion::ReferenceVersion(genie::util::BitReader& reader) {
    major = reader.readBypassBE<uint16_t>();
    minor = reader.readBypassBE<uint16_t>();
    patch = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::ReferenceVersion::write(genie::util::BitWriter& writer) const {
    writer.writeBypassBE(major);
    writer.writeBypassBE(minor);
    writer.writeBypassBE(patch);
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getMajor() const { return major; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getMinor() const { return minor; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getPatch() const { return patch; }

// ---------------------------------------------------------------------------------------------------------------------

bool Reference::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const Reference&>(info);
    return dataset_group_ID == other.dataset_group_ID && reference_ID == other.reference_ID &&
           reference_name == other.reference_name && ref_version == other.ref_version && sequences == other.sequences &&
           reference_location == other.reference_location && version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(util::BitReader& reader, genie::core::MPEGMinorVersion _version) : ref_version(0, 0, 0) {
    version = _version;
    dataset_group_ID = reader.readBypassBE<uint8_t>();
    reference_ID = reader.readBypassBE<uint8_t>();
    reader.readBypass_null_terminated(reference_name);
    ref_version = ReferenceVersion(reader);
    auto seq_count = reader.readBypassBE<uint16_t>();
    for (size_t i = 0; i < seq_count; ++i) {
        sequences.emplace_back(reader, version);
    }

    reference_location = ReferenceLocation::referenceLocationFactory(reader, seq_count, _version);
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name, ReferenceVersion _ref_version,
                     std::unique_ptr<ReferenceLocation> location, genie::core::MPEGMinorVersion _version)
    : dataset_group_ID(group_id),
      reference_ID(ref_id),
      reference_name(std::move(ref_name)),
      ref_version(_ref_version),
      reference_location(std::move(location)),
      version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Reference::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Reference::getReferenceID() const { return reference_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getReferenceName() const { return reference_name; }

// ---------------------------------------------------------------------------------------------------------------------

const Reference::ReferenceVersion& Reference::getRefVersion() const { return ref_version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Reference::ReferenceSeq>& Reference::getSequences() const { return sequences; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getKey() const {
    static const std::string key = "rfgn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addSequence(ReferenceSeq seq, std::string checksum) {
    sequences.emplace_back(std::move(seq));
    if (reference_location->isExternal()) {
        dynamic_cast<ExternalReferenceLocation&>(*reference_location).addChecksum(std::move(checksum));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const ReferenceLocation& Reference::getLocation() const { return *reference_location; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::write(genie::util::BitWriter& writer) const {
    GenInfo::write(writer);
    writer.writeBypassBE(dataset_group_ID);
    writer.writeBypassBE(reference_ID);
    writer.writeBypass(reference_name.data(), reference_name.length());
    ref_version.write(writer);
    writer.writeBypassBE<uint16_t>(sequences.size());
    for (const auto& s : sequences) {
        s.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

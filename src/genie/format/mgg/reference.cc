/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference.h"
#include <sstream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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
    return v_major == other.v_major && v_minor == other.v_minor && v_patch == other.v_patch;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceVersion::ReferenceVersion(uint16_t _v_major, uint16_t _v_minor, uint16_t _v_patch)
    : v_major(_v_major), v_minor(_v_minor), v_patch(_v_patch) {}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceVersion::ReferenceVersion(genie::util::BitReader& reader) {
    v_major = reader.readBypassBE<uint16_t>();
    v_minor = reader.readBypassBE<uint16_t>();
    v_patch = reader.readBypassBE<uint16_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::ReferenceVersion::write(genie::util::BitWriter& writer) const {
    writer.writeBypassBE(v_major);
    writer.writeBypassBE(v_minor);
    writer.writeBypassBE(v_patch);
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getMajor() const { return v_major; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getMinor() const { return v_minor; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::ReferenceVersion::getPatch() const { return v_patch; }

// ---------------------------------------------------------------------------------------------------------------------

bool Reference::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const Reference&>(info);
    return reference_name == other.reference_name && ref_version == other.ref_version && sequences == other.sequences &&
           reference_location == other.reference_location && version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(util::BitReader& reader, genie::core::MPEGMinorVersion _version) : ref_version(0, 0, 0) {
    version = _version;
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    dataset_group_ID = reader.readBypassBE<uint8_t>();
    reference_ID = reader.readBypassBE<uint8_t>();
    reader.readBypass_null_terminated(reference_name);
    ref_version = ReferenceVersion(reader);
    auto seq_count = reader.readBypassBE<uint16_t>();
    for (size_t i = 0; i < seq_count; ++i) {
        sequences.emplace_back(reader, version);
    }

    reference_location = ReferenceLocation::referenceLocationFactory(reader, seq_count, _version);
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()), "Invalid length");
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

void Reference::box_write(genie::util::BitWriter& writer) const {
    writer.writeBypassBE(dataset_group_ID);
    writer.writeBypassBE(reference_ID);
    writer.writeBypass(reference_name.data(), reference_name.length());
    ref_version.write(writer);
    writer.writeBypassBE<uint16_t>(static_cast<uint16_t>(sequences.size()));
    for (const auto& s : sequences) {
        s.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::ReferenceSeq::ReferenceSeq(genie::core::meta::Sequence s, genie::core::MPEGMinorVersion _version)
    : name(std::move(s.getName())),
      sequence_length(static_cast<uint32_t>(s.getLength())),
      sequence_id(s.getID()),
      version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::patchID(uint8_t groupID) { dataset_group_ID = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

void Reference::patchRefID(uint8_t _old, uint8_t _new) {
    if (reference_ID == _old) {
        reference_ID = _new;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::meta::Reference Reference::decapsulate(std::string meta) {
    std::unique_ptr<genie::core::meta::RefBase> location = reference_location->decapsulate();
    genie::core::meta::Reference ret(std::move(reference_name), ref_version.getMajor(), ref_version.getMinor(),
                                     ref_version.getPatch(), std::move(location), std::move(meta));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(uint8_t _dataset_group_id, uint8_t _reference_ID, genie::core::meta::Reference ref,
                     genie::core::MPEGMinorVersion _version)
    : dataset_group_ID(_dataset_group_id),
      reference_ID(_reference_ID),
      reference_name(std::move(ref.getName())),
      ref_version(static_cast<uint16_t>(ref.getMajorVersion()), static_cast<uint16_t>(ref.getMinorVersion()),
                  static_cast<uint16_t>(ref.getPatchVersion())),
      version(_version) {
    for (auto& r : ref.getSequences()) {
        sequences.emplace_back(std::move(r), version);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Reference");
    print_offset(output, depth + 1, max_depth,
                 "Dataset group ID: " + std::to_string(static_cast<int>(dataset_group_ID)));
    print_offset(output, depth + 1, max_depth, "Reference ID: " + std::to_string(static_cast<int>(reference_ID)));
    print_offset(output, depth + 1, max_depth, "Reference name: " + reference_name);
    print_offset(output, depth + 1, max_depth, "Reference major version: " + std::to_string(ref_version.getMajor()));
    print_offset(output, depth + 1, max_depth, "Reference minor version: " + std::to_string(ref_version.getMinor()));
    print_offset(output, depth + 1, max_depth, "Reference patch version: " + std::to_string(ref_version.getPatch()));
    for (const auto& r : sequences) {
        std::string s = "Reference sequence: " + r.getName();
        if (version != core::MPEGMinorVersion::V1900) {
            s += " (ID: " + std::to_string(r.getID()) + "; length: " + std::to_string(r.getLength()) + ")";
        }
        print_offset(output, depth + 1, max_depth, s);
    }
    std::string location;
    if (reference_location->isExternal()) {
        location = "External at " + dynamic_cast<const ExternalReferenceLocation&>(*reference_location).getURI();
    } else {
        const auto& i = dynamic_cast<const InternalReferenceLocation&>(*reference_location);
        location = "Internal at (Dataset Group " + std::to_string(static_cast<int>(i.getDatasetGroupID())) +
                   ", Dataset " + std::to_string(static_cast<int>(i.getDatasetID())) + ")";
    }
    print_offset(output, depth + 1, max_depth, "Reference location: " + location);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

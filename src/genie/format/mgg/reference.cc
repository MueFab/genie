/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference.h"
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/format/mgg/reference/location/external.h"
#include "genie/format/mgg/reference/location/internal.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

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

Reference::Reference(util::BitReader& reader, core::MPEGMinorVersion _version) : ref_version(0, 0, 0) {
    version = _version;
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    dataset_group_ID = reader.readAlignedInt<uint8_t>();
    reference_ID = reader.readAlignedInt<uint8_t>();
    reference_name = reader.readAlignedStringTerminated();
    ref_version = reference::Version(reader);
    auto seq_count = reader.readAlignedInt<uint16_t>();
    for (size_t i = 0; i < seq_count; ++i) {
        sequences.emplace_back(reader, version);
    }

    reference_location = reference::Location::factory(reader, seq_count, _version);
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name, reference::Version _ref_version,
                     std::unique_ptr<reference::Location> location, core::MPEGMinorVersion _version)
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

const reference::Version& Reference::getRefVersion() const { return ref_version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<reference::Sequence>& Reference::getSequences() const { return sequences; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getKey() const {
    static const std::string key = "rfgn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addSequence(reference::Sequence seq, std::string checksum) {
    sequences.emplace_back(std::move(seq));
    if (reference_location->isExternal()) {
        dynamic_cast<reference::location::External&>(*reference_location).addChecksum(std::move(checksum));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const reference::Location& Reference::getLocation() const { return *reference_location; }

// ---------------------------------------------------------------------------------------------------------------------

void Reference::box_write(util::BitWriter& writer) const {
    writer.writeAlignedInt(dataset_group_ID);
    writer.writeAlignedInt(reference_ID);
    writer.writeAlignedBytes(reference_name.data(), reference_name.length());
    writer.writeAlignedInt<uint8_t>(0);
    ref_version.write(writer);
    writer.writeAlignedInt<uint16_t>(static_cast<uint16_t>(sequences.size()));
    for (const auto& s : sequences) {
        s.write(writer);
    }
    reference_location->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::patchID(uint8_t groupID) { dataset_group_ID = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

void Reference::patchRefID(uint8_t _old, uint8_t _new) {
    if (reference_ID == _old) {
        reference_ID = _new;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::meta::Reference Reference::decapsulate(std::string meta) {
    std::unique_ptr<core::meta::RefBase> location = reference_location->decapsulate();
    core::meta::Reference ret(std::move(reference_name), ref_version.getMajor(), ref_version.getMinor(),
                                     ref_version.getPatch(), std::move(location), std::move(meta));
    for (auto& s : sequences) {
        ret.addSequence(core::meta::Sequence(s.getName(), s.getLength(), s.getID()));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(uint8_t _dataset_group_id, uint8_t _reference_ID, core::meta::Reference ref,
                     core::MPEGMinorVersion _version)
    : dataset_group_ID(_dataset_group_id),
      reference_ID(_reference_ID),
      reference_name(std::move(ref.getName())),
      ref_version(static_cast<uint16_t>(ref.getMajorVersion()), static_cast<uint16_t>(ref.getMinorVersion()),
                  static_cast<uint16_t>(ref.getPatchVersion())),
      version(_version) {
    for (auto& r : ref.getSequences()) {
        sequences.emplace_back(std::move(r), version);
    }
    reference_location = reference::Location::referenceLocationFactory(ref.moveBase(), _version);
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Reference");
    print_offset(output, depth + 1, max_depth,
                 "Dataset group ID: " + std::to_string(dataset_group_ID));
    print_offset(output, depth + 1, max_depth, "Reference ID: " + std::to_string(reference_ID));
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
        location = "External at " + dynamic_cast<const reference::location::External&>(*reference_location).getURI();
    } else {
        const auto& i = dynamic_cast<const reference::location::Internal&>(*reference_location);
        location = "Internal at (Dataset Group " + std::to_string(i.getDatasetGroupID()) +
                   ", Dataset " + std::to_string(i.getDatasetID()) + ")";
    }
    print_offset(output, depth + 1, max_depth, "Reference location: " + location);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

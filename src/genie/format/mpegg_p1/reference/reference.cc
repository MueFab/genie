/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include "genie/format/mpegg_p1/reference/reference.h"
#include "genie/util/make-unique.h"
#include "genie/format/mpegg_p1/reference/reference_location/internal.h"
#include "genie/format/mpegg_p1/reference/reference_location/external.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference()
    : dataset_group_ID(0),
      reference_ID(0),
      reference_name(),
      reference_major_version(0),
      reference_minor_version(0),
      reference_patch_version(0),
      sequence_names(),
      reference_location() {}

// ---------------------------------------------------------------------------------------------------------------------

//Reference::Reference(uint8_t _ds_group_ID, uint8_t _ref_ID, std::string _ref_name, uint16_t _ref_major_ver,
//                     uint16_t _ref_minor_ver, uint16_t _ref_patch_ver, std::vector<std::string>&& _seq_names,
//                     std::unique_ptr<ReferenceLocation> _ref_loc)
//    : ID(_ds_group_ID),
//      reference_ID(_ref_ID),
//      reference_name(std::move(_ref_name)),
//      reference_major_version(_ref_major_ver),
//      reference_minor_version(_ref_minor_ver),
//      reference_patch_version(_ref_patch_ver),
//      sequence_names(std::move(_seq_names)),
//      //      reference_location(util::make_unique<ReferenceLocation>(_ref_loc))
//      reference_location(std::move(_ref_loc)) {}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(util::BitReader& bitreader, FileHeader& fhd, size_t start_pos, size_t length):
    /// reference_ID u(8)
    dataset_group_ID(bitreader.read<uint8_t>()),
    /// reference_ID u(8)
    reference_ID(bitreader.read<uint8_t>()),
    /// reference_name st(v)
    reference_name(readNullTerminatedStr(bitreader)),
    /// reference_name u(16)
    reference_major_version(bitreader.read<uint16_t>()),
    /// reference_name u(16)
    reference_minor_version(bitreader.read<uint16_t>()),
    /// reference_name u(16)
    reference_patch_version(bitreader.read<uint16_t>()),
    sequence_names(),
    reference_location() {

    // seq_count u(16)
    auto seq_count = bitreader.read<uint16_t>();

    for (auto seqID = 0; seqID < seq_count; seqID++) {
        // sequence_name[seqID] st(v)
        sequence_names.emplace_back(readNullTerminatedStr(bitreader));

        if (fhd.getMinorVersion() != "1900"){
            sequence_lengths.emplace_back(bitreader.read<uint32_t>());
            sequence_IDs.emplace_back(bitreader.read<uint16_t>());
        }
    }

    /// reserved u(7)
    bitreader.read<uint8_t>(7);

    /// external_ref_flat u(1)
    bool external_ref_flag = bitreader.read<bool>(1);

    if (external_ref_flag) {
        reference_location = util::make_unique<External>(bitreader, fhd, seq_count);
    } else {
        reference_location = util::make_unique<Internal>(bitreader);
    }

    UTILS_DIE_IF(!bitreader.isAligned() || bitreader.getPos() - start_pos != length,
                 "Invalid Reference length!");
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::setDatasetGroupId(uint8_t _dataset_group_ID) { dataset_group_ID = _dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Reference::getDatasetGroupId() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addSequenceNames(std::vector<std::string>&& _seq_names) { sequence_names = std::move(_seq_names); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Reference::getSequenceNames() const { return sequence_names; }

// ---------------------------------------------------------------------------------------------------------------------

//void Reference::addReferenceLocation(std::unique_ptr<ReferenceLocation> _ref_loc) { reference_location = std::move(_ref_loc); }

// ---------------------------------------------------------------------------------------------------------------------

//const ReferenceLocation& Reference::getReferenceLocation() const { return reference_location; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::getSeqCount() const { return static_cast<uint16_t>(sequence_names.size()); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getLength() const {
    // length is first calculated in bits then converted in bytes

    // key c(4), Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    // ID u(8)
    len += sizeof(dataset_group_ID);

    // reference_ID u(8)
    len += sizeof(reference_ID);

    // reference_name st(v)
    len += (reference_name.size() + 1);  // // bit_len of string (stringLength + 1)*8 - Section 6.2.3

    // reference_major_version u(16)
    len += sizeof(reference_major_version);

    // reference_minor_version u(16)
    len += sizeof(reference_minor_version);

    // reference_patch_version u(16)
    len += sizeof(reference_patch_version);

    // seq_count u(16)
    len += 2;

    // sequence_name[] st(v)
    for (auto& sequence_name : sequence_names) {
        // bit_len of string in Section 6.2.3
        len += (sequence_name.size() + 1);
    }

    // reserved u(7), external_ref_flag u(1) and its content
    if (reference_location->isExternal()){
        len += dynamic_cast<External&>(*reference_location).getLength();
    } else {
        len += dynamic_cast<Internal&>(*reference_location).getLength();
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::write(util::BitWriter& writer) const {
    // KLV (Key Length Value) format

    // Key of KLV format
    writer.write("rfgn");

    // Length of KLV format
    writer.write(getLength(), 64);

    // ID u(8)
    writer.write(dataset_group_ID, 8);

    // reference_ID u(8)
    writer.write(reference_ID, 8);

    // reference_name st(v)
    writeNullTerminatedStr(writer, reference_name);

    // reference_major_version u(16)
    writer.write(reference_major_version, 16);

    // reference_minor_version u(16)
    writer.write(reference_minor_version, 16);

    // reference_patch_version u(16)
    writer.write(reference_patch_version, 16);

    // write seq_count u(16)
    writer.write(getSeqCount(), 16);

    // write sequence_name[]
    for (auto& sequence_name : sequence_names) {
        writer.write(sequence_name);
    }

    // write reserved u(7), external_ref_flag u(1) and its content
    if (reference_location->isExternal()){
        dynamic_cast<External&>(*reference_location).write(writer);
    } else {
        dynamic_cast<Internal&>(*reference_location).write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

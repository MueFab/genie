#include <utility>

#include "reference.h"
#include "reference_location/external.h"
#include "reference_location/internal.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

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

Reference::Reference(uint8_t _ds_group_ID, uint8_t _ref_ID, std::string _ref_name, uint16_t _ref_major_ver,
                     uint16_t _ref_minor_ver, uint16_t _ref_patch_ver, std::vector<std::string>&& _seq_names,
                     ReferenceLocation&& _ref_loc)
    : dataset_group_ID(_ds_group_ID),
      reference_ID(_ref_ID),
      reference_name(std::move(_ref_name)),
      reference_major_version(_ref_major_ver),
      reference_minor_version(_ref_minor_ver),
      reference_patch_version(_ref_patch_ver),
      sequence_names(std::move(_seq_names)),
      reference_location(std::move(_ref_loc))
    {}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(util::BitReader& reader, size_t length)
    : dataset_group_ID(0),
      reference_ID(0),
      reference_name(),
      reference_major_version(0),
      reference_minor_version(0),
      reference_patch_version(0),
      sequence_names(0),
      reference_location() {

    size_t start_pos = reader.getPos();

    dataset_group_ID = reader.read<uint8_t>();
    reference_ID = reader.read<uint8_t>();
    reference_name = reader.read<std::string>();
    reference_major_version = reader.read<uint16_t>();
    reference_minor_version = reader.read<uint16_t>();
    reference_patch_version = reader.read<uint16_t>();

    auto seq_count = reader.read<uint16_t>();

    for (auto seqID = 0; seqID < seq_count; seqID++){
        sequence_names.emplace_back(reader.read<std::string>());
    }

    // Reserved
    reader.read(7);

    bool external_ref_flag = reader.read(1);

    if (external_ref_flag){
        reference_location = External(reader);  //TODO(Raouf): check External(reader)
    } else {
        reference_location = Internal(reader);
    }
    
    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid DatasetGroup length!");

}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Reference::getDatasetGroupId() const {return dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addSequenceNames(std::vector<std::string>&& _seq_names) { sequence_names = std::move(_seq_names);}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Reference::getSequenceNames() const {return sequence_names;}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addReferenceLocation(ReferenceLocation&& _ref_loc) {reference_location = std::move(_ref_loc);}

// ---------------------------------------------------------------------------------------------------------------------

const ReferenceLocation &Reference::getReferenceLocation() const {return reference_location;}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::getSeqCount() const { return sequence_names.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getLength() const {

// length is first calculated in bits then converted in bytes

    // key c(4), Length u(64)
    uint64_t len = (4 * sizeof(char) + 8) * 8;  //gen_info

    // dataset_group_ID u(8)
    len += 8;

    // reference_ID u(8)
    len += 8;

    // reference_name st(v)
    len += (reference_name.size() + 1) * 8; // // bit_len of string (stringLength + 1)*8 - Section 6.2.3

    // reference_major_version u(16)
    len += 16;

    // reference_minor_version u(16)
    len += 16;

    // reference_patch_version u(16)
    len += 16;

    // seq_count u(16)
    len += 16;

    // sequence_name[] st(v)
    for (auto& sequence_name: sequence_names){
        // bit_len of string in Section 6.2.3
        len += (sequence_name.size() + 1) * 8;
    }

    // reserved u(7)
    len += 7;

    // external_ref_flag u(1)
    len += 1;

    if ( reference_location.getExternalRefFlag() == ReferenceLocation::Flag::External) {
        len += external.getBitLength();
    }

    else {
        len += internal.getBitLength();
    }

    len += len % 8;

    return len /= 8;
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::writeToFile(util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("rfgn");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // reference_ID u(8)
    bit_writer.write(reference_ID, 8);

    // reference_name st(v)
    bit_writer.write(reference_name);

    // reference_major_version u(16)
    bit_writer.write(reference_major_version, 16);

    // reference_minor_version u(16)
    bit_writer.write(reference_minor_version, 16);

    // reference_patch_version u(16)
    bit_writer.write(reference_patch_version, 16);

    // write seq_count u(16)
    bit_writer.write(getSeqCount(), 16);

    // write sequence_name[]
    for (auto& sequence_name: sequence_names){
        bit_writer.write(sequence_name);
    }

    // reserve 7 bits u(7)
    bit_writer.write(0, 7);

    // write external_ref_flag u(1)
    bit_writer.write(reference_location.isExternal(), 1);

    // if (external_ref_flag)
    reference_location.write(bit_writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
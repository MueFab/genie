#include <utility>

#include "reference.h"

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
    // key (4), Length (8)
    uint64_t len = 12;

    // TODO (Yeremia): len of Value[]?

    // dataset_group_ID u(8)
    len += 1;

    // reference_ID u(8)
    len += 1;

    // reference_name st(v)
    // bit len of string - page 11
    len += (reference_name.size() + 1) * 1;

    // reference_major_version u(16)
    len += 2;

    // reference_minor_version u(16)
    len += 2;

    // reference_patch_version u(16)
    len += 2;

    // write seq_count u(16)
    len += 2;

    // write sequence_name[]
    for (auto& sequence_name: sequence_names){
        // bit len of string - page 11
        len += (sequence_name.size() + 1) * 1;
    }

    // reserved 7 bits u(7) and external_ref_flag u(1)
    len += 1;

    // if (external_ref_flag)
    len += reference_location.getLength();

    return len;
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
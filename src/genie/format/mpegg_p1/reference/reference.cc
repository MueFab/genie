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

Reference::Reference(uint8_t _dataset_grp_ID, uint8_t _ref_ID, std::string _ref_name, uint16_t _ref_major_ver,
                     uint16_t _ref_minor_ver, uint16_t _ref_patch_ver)
    : dataset_group_ID(_dataset_grp_ID),
      reference_ID(_ref_ID),
      reference_name(std::move(_ref_name)),
      reference_major_version(_ref_major_ver),
      reference_minor_version(_ref_minor_ver),
      reference_patch_version(_ref_patch_ver),
      sequence_names(),
      reference_location()
    {}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::setSequenceNames(std::vector<std::string>&& _seq_names) {sequence_names = _seq_names;}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addReferenceLocation(ReferenceLocation&& _ref_loc) {reference_location = _ref_loc;}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Reference::getSeqCount() const { return sequence_names.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getLength() const {
    // dataset_group_ID u(8)
    uint64_t length = 1;

    // reference_ID u(8)
    length += 1;

    // reference_name st(v)
    // bit length of string - page 11
    length += (reference_name.size() + 1) * 1;

    // reference_major_version u(16)
    length += 2;

    // reference_minor_version u(16)
    length += 2;

    // reference_patch_version u(16)
    length += 2;

    // writeToFile seq_count u(16)
    length += 2;

    // writeToFile sequence_name[]
    for (auto& sequence_name: sequence_names){
        // bit length of string - page 11
        length += (sequence_name.size() + 1) * 1;
    }

    // reserved 7 bits u(7) and external_ref_flag u(1)
    length += 1;

    // if (external_ref_flag)
    length += reference_location.getLength();

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void Reference::writeToFile(util::BitWriter& bit_writer) const {

    //TODO (Yeremia): is it required?
    bit_writer.write("rfgn");

    //TODO (Yeremia): is it required?
    // length = ...

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

    // writeToFile seq_count u(16)
    bit_writer.write(getSeqCount(), 16);

    // writeToFile sequence_name[]
    for (auto& sequence_name: sequence_names){
        bit_writer.write(sequence_name);
    }

    // reserve 7 bits u(7)
    bit_writer.write(0, 7);

    // writeToFile external_ref_flag u(1)
    bit_writer.write(reference_location.isExternal(), 1);

    // if (external_ref_flag)
    reference_location.write(bit_writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
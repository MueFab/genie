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

void Reference::setSequenceNames(std::vector<std::string>&& _seq_names) {sequence_names = _seq_names;}

void Reference::addReferenceLocation(ReferenceLocation&& _ref_loc) {reference_location = _ref_loc;}

uint16_t Reference::getSeqCount() const { return sequence_names.size(); }

void Reference::write(util::BitWriter& bit_writer) const {

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

    // write seq_count
    bit_writer.write(getSeqCount(), 16);

    // write sequence_name[]
    for (auto& sequence_name: sequence_names){
        bit_writer.write(sequence_name);
    }

    // reserve 7 bits
    bit_writer.write(0, 7);

    // write external_ref_flag
    bit_writer.write(reference_location.isExternal(), 1);

    // if (external_ref_flag)
    reference_location.write(bit_writer);
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
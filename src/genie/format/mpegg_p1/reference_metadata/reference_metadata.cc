#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

#include "reference_metadata.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata()
    : dataset_group_ID(0),
      reference_ID(0) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(uint8_t _ds_group_ID, uint8_t _ref_ID)
    : dataset_group_ID(_ds_group_ID),
      reference_ID(_ref_ID) {}
//      reference_metadata_value(std::move(_ref_metadata_value)){}

// ---------------------------------------------------------------------------------------------------------------------
/*
ReferenceMetadata::ReferenceMetadata(util::BitReader& reader, size_t length)
    : dataset_group_ID(0),
      reference_ID(0)
    {
//      reference_metadata_value() reader{

    size_t start_pos = reader.getPos();

    dataset_group_ID = reader.read<uint8_t>();
    reference_ID = reader.read<uint8_t>();

//    for (auto val : reference_metadata_value) {
//        reference_metadata_value = reader.read<uint8_t>();
//    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DatasetGroup length!");
}
*/
// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceMetadata::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // dataset_group_ID u(8)
    len += 1;

    // reference_ID u(8)
    len += 1;

    // reference_metadata_value()
//    len += reference_metadata_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::write(genie::util::BitWriter& bit_writer) const {

    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("rfmd");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // reference_ID u(8)
    bit_writer.write(reference_ID, 8);

//    for (auto val: reference_metadata_value){
//        bit_writer.write(val, 8);
//    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

#include "reference_metadata.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata()
    : dataset_group_ID(0),
      reference_ID(0),
      reference_metadata_value(){

    // TODO (Yeremia): default value for reference_metadata_value
    UTILS_DIE("Not implemented yet");
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceMetadata::getLength() const {
    // key (4), Length (8)
    uint64_t len = 12;

    // TODO (Yeremia): len of Value[]?

    // dataset_group_ID u(8)
    len += 1;

    // reference_ID u(8)
    len += 1;

    // reference_metadata_value()
    len += reference_metadata_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::writeToFile(genie::util::BitWriter& bit_writer) const {

    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("rfmd");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // reference_ID u(8)
    bit_writer.write(reference_ID, 8);

    for (auto val: reference_metadata_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

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
    // dataset_group_ID u(8)
    uint64_t length = 1;

    // reference_ID u(8)
    length += 1;

    // reference_metadata_value()
    length += reference_metadata_value.size();

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::writeToFile(genie::util::BitWriter& bit_writer) const {

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

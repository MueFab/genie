#include "genie/util/runtime-exception.h"
#include "label_list.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList()
    : dataset_group_ID() {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID)
    : dataset_group_ID(_ds_group_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID, std::vector<Label> &&_labels)
    : dataset_group_ID(_ds_group_ID),
      labels(std::move(_labels)){}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::setLabels(std::vector<Label>&& _labels) {
    labels = _labels;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::setDatasetGroupId(uint8_t _dataset_group_ID) {dataset_group_ID = _dataset_group_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LabelList::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t LabelList::getNumLabels() const { return (uint16_t) labels.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Label>& LabelList::getLabels() const { return labels; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelList::getLength() const {

    // dataset_group_ID u(8)
    uint64_t length = 1;

    // num_labels u(16)
    length += 2;

    // for Label
    for (auto& label: labels){
        length += label.getLength();
    }

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::writeToFile(util::BitWriter& bit_writer) const {

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // num_labels u(16)
    bit_writer.write(getNumLabels(), 16);

    // for Label
    for (auto& label: labels){
        label.write(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
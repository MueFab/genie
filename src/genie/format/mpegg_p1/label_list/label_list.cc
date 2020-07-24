#include "genie/util/runtime-exception.h"
#include "label_list.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

LabelList::LabelList()
    : dataset_group_ID() {}

LabelList::LabelList(uint8_t _ds_group_ID)
    : dataset_group_ID(_ds_group_ID) {}

void LabelList::addLabel(Label&& _label) {
    labels.push_back(std::move(_label));
}

void LabelList::addLabels(std::vector<Label>& _labels) {
    UTILS_DIE_IF(_labels.empty(), "_label is empty!");

    std::move(_labels.begin(), _labels.end(), std::back_inserter(labels));
}

void LabelList::setLabels(std::vector<Label>&& _labels) {
    labels = _labels;
}

uint8_t LabelList::getDatasetGroupID() const { return dataset_group_ID; }

uint16_t LabelList::getNumLabels() const { return (uint16_t) labels.size(); }

const std::vector<Label>& LabelList::getLabels() const { return labels; }

void LabelList::write(util::BitWriter& bit_writer) const {

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // num_labels u(16)
    bit_writer.write(getNumLabels(), 16);

    // for Label
    for (auto& label: labels){
        label.write(bit_writer);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
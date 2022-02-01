/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/label_list.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID) : dataset_group_ID(_ds_group_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(util::BitReader& reader) {
    reader.readBypassBE<uint64_t>();
    // ID u(8)
    dataset_group_ID = reader.read<uint8_t>();
    // num_labels u(16)
    auto num_labels = reader.read<uint16_t>();

    /// Data encapsulated in Class Label
    for (size_t i = 0; i < num_labels; ++i) {
        labels.emplace_back(reader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LabelList::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Label>& LabelList::getLabels() const { return labels; }

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::box_write(util::BitWriter& bit_writer) const {
    bit_writer.writeBypassBE<uint8_t>(dataset_group_ID);
    bit_writer.writeBypassBE<uint16_t>(labels.size());
    // data encapsulated in Class Label
    for (auto& label : labels) {
        label.write(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& LabelList::getKey() const {
    static const std::string key = "labl";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

bool LabelList::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const LabelList&>(info);
    return dataset_group_ID == other.dataset_group_ID && labels == other.labels;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::addLabel(Label l) { labels.emplace_back(std::move(l)); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

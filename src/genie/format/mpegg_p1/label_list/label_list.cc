/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/label_list/label_list.h"
#include <string>
#include <utility>
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList() : dataset_group_ID(), labels() {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID) : dataset_group_ID(_ds_group_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID, std::vector<Label>&& _labels)
    : dataset_group_ID(_ds_group_ID), labels(std::move(_labels)) {}

// ---------------------------------------------------------------------------------------------------------------------
LabelList::LabelList(util::BitReader& reader, size_t length) {
    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "labl", "LabelList is not Found");

    size_t start_pos = reader.getPos();

    // dataset_group_ID u(8)
    dataset_group_ID = reader.read<uint8_t>();
    // num_labels u(16)
    reader.read<uint16_t>();

    /// Data encapsulated in Class Label
    for (auto& label : labels) {
        label.ReadLabel(reader);
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid LabelList length!");
}
// ---------------------------------------------------------------------------------------------------------------------

void LabelList::setDatasetGroupId(uint8_t _dataset_group_ID) { dataset_group_ID = _dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LabelList::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::setLabels(std::vector<Label>&& _labels) { labels = _labels; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Label>& LabelList::getLabels() const { return labels; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t LabelList::getNumLabels() const { return (uint16_t)labels.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelList::getLength() const {
    /// key c(4), Length u(64)
    uint64_t len = (4 * sizeof(char) + 8) * 1;  // gen_info

    // dataset_group_ID u(8)
    len += 1;

    // num_labels u(16)
    len += 2;

    /// data encapsulated in Class Label
    for (auto& label : labels) {
        len += label.getLength();  // gen_info
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::writeToFile(util::BitWriter& bit_writer) const {
    /// KLV (Key Length Value) format
    // Key of KVL format
    bit_writer.write("labl");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    bit_writer.write(dataset_group_ID, 8);

    // num_labels u(16)
    bit_writer.write(getNumLabels(), 16);

    /// data encapsulated in Class Label
    for (auto& label : labels) {
        label.write(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

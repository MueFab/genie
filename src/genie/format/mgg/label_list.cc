/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/label_list.h"
#include <utility>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(uint8_t _ds_group_ID) : dataset_group_ID(_ds_group_ID) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelList::LabelList(util::BitReader& reader) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    // ID u(8)
    dataset_group_ID = reader.readBypassBE<uint8_t>();
    // num_labels u(16)
    auto num_labels = reader.readBypassBE<uint16_t>();

    for (size_t i = 0; i < num_labels; ++i) {
        read_box(reader, false);
    }
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LabelList::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Label>& LabelList::getLabels() const { return labels; }

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::box_write(util::BitWriter& bit_writer) const {
    bit_writer.writeBypassBE<uint8_t>(dataset_group_ID);
    bit_writer.writeBypassBE<uint16_t>(static_cast<uint16_t>(labels.size()));
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

void LabelList::patchID(uint8_t groupID) { dataset_group_ID = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<genie::core::meta::Label> LabelList::decapsulate(uint16_t dataset) {
    std::vector<genie::core::meta::Label> ret;
    for (auto& l : labels) {
        ret.emplace_back(l.decapsulate(dataset));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Label list");
    for (const auto& l : labels) {
        l.print_debug(output, depth + 1, max_depth);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelList::read_box(util::BitReader& reader, bool in_offset) {
    std::string tmp_str(4, '\0');
    reader.readBypass(tmp_str);
    if (tmp_str == "lbll") {
        labels.emplace_back(reader);
    } else if (tmp_str == "offs") {
        UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
        reader.readBypass(tmp_str);
        uint64_t offset = reader.readBypassBE<uint64_t>();
        if (offset == ~static_cast<uint64_t>(0)) {
            read_box(reader, in_offset);
            return;
        }
        auto pos_save = reader.getPos();
        reader.setPos(offset);
        read_box(reader, true);
        reader.setPos(pos_save);
    } else {
        UTILS_DIE("Unknown box");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

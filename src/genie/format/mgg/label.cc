/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/label.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label() : label_ID(), dataset_infos() {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string _label_ID) : label_ID(std::move(_label_ID)) {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(util::BitReader& reader) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    label_ID = reader.readAlignedStringTerminated();
    auto num_datasets = reader.read<uint16_t>();

    for (size_t i = 0; i < num_datasets; ++i) {
        dataset_infos.emplace_back(reader);
    }
    reader.flushHeldBits();
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getStreamPosition()),
                 "Invalid length: start_pos " + std::to_string(start_pos) + "; length " + std::to_string(length) +
                     "; position should be " + std::to_string(start_pos + length) + "; position is " +
                     std::to_string(reader.getStreamPosition()));
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addDataset(LabelDataset _ds_info) { dataset_infos.push_back(std::move(_ds_info)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getLabelID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void Label::box_write(util::BitWriter& bit_writer) const {
    bit_writer.writeAlignedBytes(label_ID.data(), label_ID.length());
    bit_writer.writeAlignedInt('\0');

    // num_datasets u(16)
    bit_writer.writeAlignedInt<uint16_t>(static_cast<uint16_t>(dataset_infos.size()));

    // data encapsulated in Class dataset_info
    for (auto& ds_info : dataset_infos) {
        ds_info.write(bit_writer);
    }

    // aligned to byte
    bit_writer.flushBits();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getKey() const {
    static const std::string key = "lbll";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Label::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const Label&>(info);
    return label_ID == other.label_ID && dataset_infos == other.dataset_infos;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<LabelDataset>& Label::getDatasets() const { return dataset_infos; }

// ---------------------------------------------------------------------------------------------------------------------

core::meta::Label Label::decapsulate(uint16_t dataset) {
    core::meta::Label ret(label_ID);
    for (auto& d : dataset_infos) {
        auto regions = d.decapsulate(dataset);
        for (auto& r : regions) {
            ret.addRegion(std::move(r));
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Label");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

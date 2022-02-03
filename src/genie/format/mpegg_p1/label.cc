/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/label.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label() : label_ID(), dataset_infos() {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string _label_ID) : label_ID(std::move(_label_ID)) {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(util::BitReader& reader) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    reader.readBypass_null_terminated(label_ID);
    auto num_datasets = reader.read<uint16_t>();

    for (size_t i = 0; i < num_datasets; ++i) {
        dataset_infos.emplace_back(reader);
    }
    reader.flush();
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()),
                 "Invalid length: start_pos " + std::to_string(start_pos) + "; length " + std::to_string(length) +
                     "; position should be " + std::to_string(start_pos + length) + "; position is " +
                     std::to_string(reader.getPos()));
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addDataset(LabelDataset _ds_info) { dataset_infos.push_back(std::move(_ds_info)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getLabelID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void Label::box_write(util::BitWriter& bit_writer) const {
    bit_writer.writeBypass(label_ID.data(), label_ID.length());
    bit_writer.writeBypassBE('\0');

    // num_datasets u(16)
    bit_writer.writeBypassBE<uint16_t>(dataset_infos.size());

    // data encapsulated in Class dataset_info
    for (auto& ds_info : dataset_infos) {
        ds_info.write(bit_writer);
    }

    // aligned to byte
    bit_writer.flush();
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "label.h"
#include <utility>
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label() : label_ID(), dataset_infos() {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string _label_ID) : label_ID(std::move(_label_ID)) {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(util::BitReader& reader) {
    reader.readBypassBE<uint64_t>();
    reader.readBypass_null_terminated(label_ID);
    auto num_datasets = reader.read<uint16_t>();

    for (size_t i = 0; i < num_datasets; ++i) {
        dataset_infos.emplace_back(reader);
    }
    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addDataset(LabelDataset _ds_info) { dataset_infos.push_back(std::move(_ds_info)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getLabelID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void Label::write(util::BitWriter& bit_writer) const {
    GenInfo::write(bit_writer);

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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

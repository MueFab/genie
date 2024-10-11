/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_dataset.h"
#include <filesystem>  // NOLINT
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

EncapsulatedDataset::EncapsulatedDataset(const std::string& input_file, core::MPEGMinorVersion version)
    : reader(input_file), mgb_file(&reader) {
    if (std::filesystem::exists(input_file + ".json") && std::filesystem::file_size(input_file + ".json")) {
        std::ifstream in_file(input_file + ".json");
        nlohmann::json my_json;
        in_file >> my_json;
        meta = core::meta::Dataset(my_json);
    }
    for (uint8_t multiple_alignment = 0; multiple_alignment < 2; ++multiple_alignment) {
        for (uint8_t pos40 = 0; pos40 < 2; ++pos40) {
            for (uint8_t data_type = 0;
                 data_type < static_cast<uint8_t>(core::parameter::DataUnit::DatasetType::COUNT); ++data_type) {
                for (uint8_t alphabet_type = 0; alphabet_type < static_cast<uint8_t>(core::AlphabetID::COUNT);
                     ++alphabet_type) {
                    auto param_ids = mgb_file.collect_param_ids(
                        static_cast<bool>(multiple_alignment), static_cast<bool>(pos40),
                        static_cast<core::parameter::DataUnit::DatasetType>(data_type),
                        static_cast<core::AlphabetID>(alphabet_type));
                    if (param_ids.empty()) {
                        continue;
                    }
                    auto dataset = Dataset(mgb_file, meta, version, param_ids);
                    datasets.emplace_back(std::move(dataset));
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

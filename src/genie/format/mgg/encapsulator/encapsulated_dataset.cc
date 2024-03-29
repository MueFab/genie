/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_dataset.h"
#include <utility>
#include "filesystem/filesystem.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

EncapsulatedDataset::EncapsulatedDataset(const std::string& input_file, genie::core::MPEGMinorVersion version)
    : reader(input_file), mgb_file(&reader) {
    if ((ghc::filesystem::exists(input_file + ".json") && ghc::filesystem::file_size(input_file + ".json"))) {
        std::ifstream in_file(input_file + ".json");
        nlohmann::json my_json;
        in_file >> my_json;
        meta = genie::core::meta::Dataset(my_json);
    }
    for (uint8_t multiple_alignment = 0; multiple_alignment < 2; ++multiple_alignment) {
        for (uint8_t pos40 = 0; pos40 < 2; ++pos40) {
            for (uint8_t data_type = 0; data_type < uint8_t(genie::core::parameter::DataUnit::DatasetType::COUNT);
                 ++data_type) {
                for (uint8_t alphabet_type = 0; alphabet_type < uint8_t(genie::core::AlphabetID::COUNT);
                     ++alphabet_type) {
                    auto param_ids =
                        mgb_file.collect_param_ids(static_cast<bool>(multiple_alignment), static_cast<bool>(pos40),
                                                   genie::core::parameter::DataUnit::DatasetType(data_type),
                                                   genie::core::AlphabetID(alphabet_type));
                    if (param_ids.empty()) {
                        continue;
                    }
                    auto dataset = genie::format::mgg::Dataset(mgb_file, meta, version, param_ids);
                    datasets.emplace_back(std::move(dataset));
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace encapsulator
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

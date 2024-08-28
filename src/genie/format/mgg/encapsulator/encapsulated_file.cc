/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_file.h"
#include <utility>
#include <filesystem>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

std::map<uint8_t, std::vector<std::string>> EncapsulatedFile::groupInputFiles(
    const std::vector<std::string>& input_files) const {
    std::vector<std::string> unknown_id;
    std::map<uint8_t, std::vector<std::string>> ret;
    for (auto& i : input_files) {
        if (!std::filesystem::exists(i + ".json")) {
            unknown_id.emplace_back(i);
            continue;
        }

        if ((std::filesystem::exists(i + ".json") && std::filesystem::file_size(i + ".json"))) {
            std::ifstream in_file(i + ".json");
            nlohmann::json my_json;
            in_file >> my_json;
            genie::core::meta::Dataset dataset(my_json);
            if (dataset.getDataGroup() == boost::none) {
                unknown_id.emplace_back(i);
                continue;
            }
            ret[static_cast<uint8_t>(dataset.getDataGroup()->getID())].emplace_back(i);
        } else {
            unknown_id.emplace_back(i);
        }
    }

    if (!unknown_id.empty()) {
        for (size_t i = 0; i < input_files.size(); ++i) {
            if (ret.find(static_cast<uint8_t>(i)) == ret.end()) {
                ret[static_cast<uint8_t>(i)] = std::move(unknown_id);
                break;
            }
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

EncapsulatedFile::EncapsulatedFile(const std::vector<std::string>& input_files, genie::core::MPEGMinorVersion version) {
    std::map<uint8_t, std::vector<std::string>> file_groups = groupInputFiles(input_files);

    for (auto& g : file_groups) {
        auto group = EncapsulatedDatasetGroup(g.second, version);
        group.patchID(g.first);
        groups.emplace_back(std::move(group));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

genie::format::mgg::MggFile EncapsulatedFile::assemble(genie::core::MPEGMinorVersion version) {
    genie::format::mgg::MggFile ret;
    ret.addBox(genie::util::make_unique<genie::format::mgg::FileHeader>(version));

    for (auto& g : groups) {
        ret.addBox(genie::util::make_unique<genie::format::mgg::DatasetGroup>(g.assemble(version)));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace encapsulator
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

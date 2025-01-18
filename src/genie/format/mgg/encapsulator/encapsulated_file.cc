/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_file.h"

#include <filesystem>  // NOLINT
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/format/mgg/file_header.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// -----------------------------------------------------------------------------

std::map<uint8_t, std::vector<std::string>> EncapsulatedFile::GroupInputFiles(
    const std::vector<std::string>& input_files) {
  std::vector<std::string> unknown_id;
  std::map<uint8_t, std::vector<std::string>> ret;
  for (auto& i : input_files) {
    if (!std::filesystem::exists(i + ".json")) {
      unknown_id.emplace_back(i);
      continue;
    }

    if (std::filesystem::exists(i + ".json") &&
        std::filesystem::file_size(i + ".json")) {
      std::ifstream in_file(i + ".json");
      UTILS_DIE_IF(!in_file, "Cannot open file to read: " + i + ".json");
      nlohmann::json my_json;
      in_file >> my_json;
      core::meta::Dataset dataset(my_json);
      if (dataset.GetDataGroup() == std::nullopt) {
        unknown_id.emplace_back(i);
        continue;
      }
      ret[static_cast<uint8_t>(dataset.GetDataGroup()->GetId())].emplace_back(
          i);
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

// -----------------------------------------------------------------------------

EncapsulatedFile::EncapsulatedFile(const std::vector<std::string>& input_files,
                                   const core::MpegMinorVersion version) {
  const std::map<uint8_t, std::vector<std::string>> file_groups =
      GroupInputFiles(input_files);

  for (const auto& [fst, snd] : file_groups) {
    auto group = EncapsulatedDatasetGroup(snd, version);
    group.PatchId(fst);
    groups.emplace_back(std::move(group));
  }
}

// -----------------------------------------------------------------------------

MggFile EncapsulatedFile::assemble(core::MpegMinorVersion version) {  // NOLINT
  MggFile ret;
  ret.AddBox(std::make_unique<FileHeader>(version));

  for (auto& g : groups) {
    ret.AddBox(std::make_unique<DatasetGroup>(g.Assemble(version)));
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

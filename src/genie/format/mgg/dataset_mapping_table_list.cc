/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_mapping_table_list.h"

#include <string>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool DataSetMappingTableList::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DataSetMappingTableList&>(info);
  return dataset_group_ID == other.dataset_group_ID &&
         dataset_mapping_table_SID == other.dataset_mapping_table_SID;
}

// -----------------------------------------------------------------------------
const std::string& DataSetMappingTableList::GetKey() const {
  static const std::string key = "dmtl";
  return key;
}

// -----------------------------------------------------------------------------
DataSetMappingTableList::DataSetMappingTableList(const uint8_t ds_group_id)
    : dataset_group_ID(ds_group_id) {}

// -----------------------------------------------------------------------------
DataSetMappingTableList::DataSetMappingTableList(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  dataset_group_ID = reader.ReadAlignedInt<uint8_t>();
  const size_t num_sids = (length - 13) / 2;
  for (size_t i = 0; i < num_sids; ++i) {
    dataset_mapping_table_SID.emplace_back(reader.ReadAlignedInt<uint16_t>());
  }
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
void DataSetMappingTableList::AddDatasetMappingTableSid(uint16_t sid) {
  dataset_mapping_table_SID.emplace_back(sid);
}

// -----------------------------------------------------------------------------
void DataSetMappingTableList::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedInt(dataset_group_ID);
  for (const auto& s : dataset_mapping_table_SID) {
    bit_writer.WriteAlignedInt(s);
  }
}

// -----------------------------------------------------------------------------
uint8_t DataSetMappingTableList::GetDatasetGroupId() const {
  return dataset_group_ID;
}

// -----------------------------------------------------------------------------
const std::vector<uint16_t>&
DataSetMappingTableList::GetDatasetMappingTableSiDs() const {
  return dataset_mapping_table_SID;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

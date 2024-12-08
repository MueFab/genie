/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_mapping_table.h"

#include <string>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
const std::string& DataSetMappingTable::GetKey() const {
  static const std::string key = "dmtb";
  return key;
}

// -----------------------------------------------------------------------------
void DataSetMappingTable::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedInt(dataset_id_);
  for (const auto& s : streams_) {
    s.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------
uint16_t DataSetMappingTable::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------
const std::vector<DataStream>& DataSetMappingTable::GetDataStreams() const {
  return streams_;
}

// -----------------------------------------------------------------------------
void DataSetMappingTable::AddDataStream(DataStream d) {
  streams_.emplace_back(d);
}

// -----------------------------------------------------------------------------
DataSetMappingTable::DataSetMappingTable(const uint16_t dataset_id)
    : dataset_id_(dataset_id) {}

// -----------------------------------------------------------------------------
DataSetMappingTable::DataSetMappingTable(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  dataset_id_ = reader.ReadAlignedInt<uint16_t>();
  const size_t num_data_streams = (length - 14) / 3;
  for (size_t i = 0; i < num_data_streams; ++i) {
    streams_.emplace_back(reader);
  }
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
bool DataSetMappingTable::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DataSetMappingTable&>(info);
  return dataset_id_ == other.dataset_id_ && streams_ == other.streams_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

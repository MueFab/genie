/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_
#define SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

template <typename Lambda>
bool MgbFile::base_sorter(
    const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>& u1,
    const std::pair<uint64_t, std::unique_ptr<core::parameter::DataUnit>>& u2,
    Lambda lambda) {
  if (data_unit_order(*u1.second) != data_unit_order(*u2.second)) {
    return data_unit_order(*u1.second) < data_unit_order(*u2.second);
  }
  if (u1.second->GetDataUnitType() ==
      core::parameter::DataUnit::DataUnitType::kParameterSet) {
    return dynamic_cast<const core::parameter::ParameterSet&>(*u1.second)
               .GetId() <
           dynamic_cast<const core::parameter::ParameterSet&>(*u2.second)
               .GetId();
  }
  if (u1.second->GetDataUnitType() ==
      core::parameter::DataUnit::DataUnitType::kRawReference) {
    const auto& r1 = dynamic_cast<const RawReference&>(*u1.second);
    const auto& r2 = dynamic_cast<const RawReference&>(*u2.second);
    if (r1.begin()->GetSeqId() != r2.begin()->GetSeqId()) {
      return r1.begin()->GetSeqId() < r2.begin()->GetSeqId();
    }
    if (r1.begin()->GetStart() != r2.begin()->GetStart()) {
      return r1.begin()->GetStart() < r2.begin()->GetStart();
    }
    return r1.begin()->GetEnd() < r2.begin()->GetEnd();
  }
  if (u1.second->GetDataUnitType() ==
      core::parameter::DataUnit::DataUnitType::kAccessUnit) {
    const auto& r1 = dynamic_cast<const AccessUnit&>(*u1.second);
    const auto& r2 = dynamic_cast<const AccessUnit&>(*u2.second);
    return lambda(r1, r2);
  }
  return false;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

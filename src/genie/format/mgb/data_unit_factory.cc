/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/data_unit_factory.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mgb/reference.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Mgb";

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

DataUnitFactory::DataUnitFactory(core::ReferenceManager* mgr,
                                 Importer* importer, const bool ref)
    : refmgr_(mgr), importer_(importer), reference_only_(ref) {}

// -----------------------------------------------------------------------------

const core::parameter::EncodingSet& DataUnitFactory::GetParams(
    const size_t id) const {
  return parameters_.at(id);
}

// -----------------------------------------------------------------------------

std::optional<AccessUnit> DataUnitFactory::read(util::BitReader& bit_reader) {
  int i = 0;
  do {
    const auto type =
        bit_reader.Read<core::parameter::DataUnit::DataUnitType>();
    size_t pos = bit_reader.GetStreamPosition();
    if (!bit_reader.IsStreamGood()) {
      bit_reader.ClearStreamState();
      return std::nullopt;
    }
    switch (type) {
      case core::parameter::DataUnit::DataUnitType::kRawReference: {
        pos += 10;
        auto r = RawReference(bit_reader, true);
        for (auto& ref : r) {
          pos += 12;
          UTILS_LOG(util::Logger::Severity::INFO,
                    "Found ref(raw) " + std::to_string(ref.GetSeqId()) + ":[" +
                        std::to_string(ref.GetStart()) + ", " +
                        std::to_string(ref.GetEnd()) + "] ...");
          refmgr_->ValidateRefId(ref.GetSeqId());
          refmgr_->AddRef(
              i++, std::make_unique<Reference>(refmgr_->Id2Ref(ref.GetSeqId()),
                                               ref.GetStart(), ref.GetEnd() + 1,
                                               importer_, pos, true));
          pos += ref.GetEnd() - ref.GetStart() + 1;
        }
        break;
      }
      case core::parameter::DataUnit::DataUnitType::kParameterSet: {
        auto p = core::parameter::ParameterSet(bit_reader);
        UTILS_LOG(util::Logger::Severity::INFO,
                  "Found PS " + std::to_string(p.GetId()) + "...");
        parameters_.insert(
            std::make_pair(p.GetId(), std::move(p.GetEncodingSet())));
        break;
      }
      case core::parameter::DataUnit::DataUnitType::kAccessUnit: {
        if (auto ret = AccessUnit(parameters_, bit_reader, true);
            GetParams(ret.GetHeader().GetParameterId()).GetDatasetType() ==
            AccessUnit::DatasetType::kReference) {
          const auto& ref = ret.GetHeader().GetRefCfg();
          refmgr_->ValidateRefId(ref.GetSeqId());
          UTILS_LOG(util::Logger::Severity::INFO,
                    "Found ref(compressed) " + std::to_string(ref.GetSeqId()) +
                        ":[" + std::to_string(ref.GetStart()) + ", " +
                        std::to_string(ref.GetEnd()) + "] ...");
          refmgr_->AddRef(
              i++, std::make_unique<Reference>(refmgr_->Id2Ref(ref.GetSeqId()),
                                               ref.GetStart(), ref.GetEnd() + 1,
                                               importer_, pos, false));
          bit_reader.SkipAlignedBytes(ret.GetPayloadSize());
        } else {
          if (!reference_only_) {
            ret.LoadPayload(bit_reader);
            for (auto& b : ret.GetBlocks()) {
              b.load();
              b.parse();
            }
            UTILS_DIE_IF(ret.GetHeader().GetClass() ==
                             genie::core::record::ClassType::kClassHm,
                         "Class HM not supported");
            UTILS_LOG(util::Logger::Severity::INFO,
                      ret.DebugPrint(
                          parameters_.at(ret.GetHeader().GetParameterId())));
            return ret;
          }
          bit_reader.SkipAlignedBytes(ret.GetPayloadSize());
        }
        break;
      }
      default: {
        UTILS_DIE("DataUnitFactory invalid DataUnitType!");
      }
    }
  } while (true);
}

// -----------------------------------------------------------------------------

const std::map<size_t, core::parameter::EncodingSet>&
DataUnitFactory::GetParams() const {
  return parameters_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

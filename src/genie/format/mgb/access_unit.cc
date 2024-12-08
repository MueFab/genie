/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/access_unit.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/parameter/parameter_set.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------
void AccessUnit::DebugPrint(const core::parameter::EncodingSet& ps) const {
  const std::string lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
  std::cerr << "AU " << header.GetId() << ": class "
            << lut[static_cast<int>(header.GetClass())];
  if (header.GetClass() != core::record::ClassType::kClassU) {
    std::cerr << ", Position [" << header.GetAlignmentInfo().GetRefId() << "-"
              << header.GetAlignmentInfo().GetStartPos() << ":"
              << header.GetAlignmentInfo().GetEndPos() << "]";
  }
  std::cerr << ", " << header.GetReadCount() << " records";

  if (header.GetClass() == core::record::ClassType::kClassU) {
    if (!ps.IsComputedReference()) {
      std::cerr << " (Low Latency)";
    } else {
      if (ps.GetComputedRef().GetAlgorithm() ==
          core::parameter::ComputedRef::Algorithm::kGlobalAssembly) {
        std::cerr << " (Global Assembly)";
      } else {
        UTILS_DIE("Computed ref not supported: " +
                  std::to_string(
                      static_cast<int>(ps.GetComputedRef().GetAlgorithm())));
      }
    }
  } else {
    if (!ps.IsComputedReference()) {
      std::cerr << " (Reference)";
    } else {
      if (ps.GetComputedRef().GetAlgorithm() ==
          core::parameter::ComputedRef::Algorithm::kLocalAssembly) {
        std::cerr << " (Local Assembly)";
      } else {
        UTILS_DIE("Computed ref not supported: " +
                  std::to_string(
                      static_cast<int>(ps.GetComputedRef().GetAlgorithm())));
      }
    }
  }
  std::cerr << "..." << std::endl;
}

// -----------------------------------------------------------------------------
void AccessUnit::LoadPayload(util::BitReader& bit_reader) {
  for (size_t i = 0; i < header.GetNumBlocks(); ++i) {
    blocks.emplace_back(qv_payloads, bit_reader);
  }
}

// -----------------------------------------------------------------------------
size_t AccessUnit::GetPayloadSize() const { return payloadbytes; }

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(
    const std::map<size_t, core::parameter::EncodingSet>& parameter_sets,
    util::BitReader& bit_reader, const bool lazy_payload)
    : DataUnit(DataUnitType::kAccessUnit) {
  UTILS_DIE_IF(!bit_reader.IsByteAligned(), "Bit reader not aligned");
  const uint64_t bit_reader_pos = bit_reader.GetTotalBitsRead() / 8 - 1;
  bit_reader.ReadBits(3);
  const auto du_size = bit_reader.Read<uint32_t>(29);

  header = AuHeader(bit_reader, parameter_sets);

  const uint64_t bytes_read =
      bit_reader.GetTotalBitsRead() / 8 - bit_reader_pos;
  payloadbytes = du_size - bytes_read;
  qv_payloads = parameter_sets.at(header.GetParameterId())
                    .GetQvConfig(header.GetClass())
                    .GetNumSubsequences();

  UTILS_DIE_IF(!bit_reader.IsByteAligned(), "Bit reader not aligned");

  if (!lazy_payload) {
    LoadPayload(bit_reader);
  }
}

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(const uint32_t access_unit_id,
                       const uint8_t parameter_set_id,
                       const core::record::ClassType au_type,
                       const uint32_t reads_count,
                       const DatasetType dataset_type, const uint8_t pos_size,
                       const bool signature_flag,
                       const core::AlphabetId alphabet)
    : DataUnit(DataUnitType::kAccessUnit),
      header(access_unit_id, parameter_set_id, au_type, reads_count,
             dataset_type, pos_size, signature_flag, alphabet),
      blocks(0),
      payloadbytes(0),
      qv_payloads(0) {
  if (au_type == core::record::ClassType::kClassN ||
      au_type == core::record::ClassType::kClassM) {
    header.SetMmCfg(MmCfg());
  }
  if (dataset_type == DatasetType::kReference) {
    header.SetRefCfg(RefCfg(pos_size));
  }
  if (au_type != core::record::ClassType::kClassU) {
    header.SetAuTypeCfg(AuTypeCfg(pos_size));
  } else {
    if (signature_flag) {
      header.SetSignatureCfg(
          SignatureCfg(GetAlphabetProperties(alphabet).base_bits));
    }
  }
}

// -----------------------------------------------------------------------------
void AuHeader::SetMmCfg(const MmCfg& cfg) {
  if (!mm_cfg_) {
    UTILS_THROW_RUNTIME_EXCEPTION("MmCfg not valid for this access unit");
  }
  mm_cfg_ = cfg;
}

// -----------------------------------------------------------------------------
void AuHeader::SetRefCfg(const RefCfg& cfg) {
  if (!ref_cfg_) {
    UTILS_THROW_RUNTIME_EXCEPTION("RefCfg not valid for this access unit");
  }
  ref_cfg_ = cfg;
}

// -----------------------------------------------------------------------------
const RefCfg& AuHeader::GetRefCfg() const { return ref_cfg_.value(); }

// -----------------------------------------------------------------------------
void AuHeader::SetAuTypeCfg(AuTypeCfg&& cfg) {
  if (!au_type_u_cfg_) {
    UTILS_THROW_RUNTIME_EXCEPTION(
        "au_type_u_cfg not valid for this access unit");
  }
  au_type_u_cfg_ = std::move(cfg);
}

// -----------------------------------------------------------------------------
void AuHeader::SetSignatureCfg(SignatureCfg&& cfg) {
  if (!signature_config_) {
    UTILS_THROW_RUNTIME_EXCEPTION(
        "signature config not valid for this access unit");
  }
  signature_config_ = std::move(cfg);
}

// -----------------------------------------------------------------------------
std::vector<Block>& AccessUnit::GetBlocks() { return blocks; }

// -----------------------------------------------------------------------------
AuHeader& AccessUnit::GetHeader() { return header; }

// -----------------------------------------------------------------------------
uint32_t AuHeader::GetId() const { return access_unit_id_; }

// -----------------------------------------------------------------------------
uint8_t AuHeader::GetParameterId() const { return parameter_set_id_; }

// -----------------------------------------------------------------------------
const AuTypeCfg& AuHeader::GetAlignmentInfo() const { return *au_type_u_cfg_; }

// -----------------------------------------------------------------------------
uint32_t AuHeader::GetReadCount() const { return reads_count_; }

// -----------------------------------------------------------------------------
core::record::ClassType AuHeader::GetClass() const { return au_type_; }

// -----------------------------------------------------------------------------
void AccessUnit::Write(util::BitWriter& writer) const {
  DataUnit::Write(writer);
  writer.WriteBits(0, 3);

  // Calculate Size and write structure to tmp buffer
  std::stringstream ss;
  util::BitWriter tmp_writer(ss);
  header.Write(tmp_writer, true);
  tmp_writer.FlushBits();
  uint64_t bits = tmp_writer.GetTotalBitsWritten();
  constexpr uint64_t type_size_size =
      8 + 3 + 29;  // data_unit_type, reserved, data_unit_size
  bits += type_size_size;
  uint64_t bytes = bits / 8;

  for (auto& i : blocks) {
    bytes += i.GetWrittenSize();
  }

  // Now Size is known, write to final destination
  writer.WriteBits(bytes, 29);
  writer.WriteAlignedStream(ss);
  for (auto& i : blocks) {
    i.Write(writer);
  }
}

// -----------------------------------------------------------------------------
void AccessUnit::AddBlock(Block block) {
  header.BlockAdded();
  blocks.push_back(std::move(block));
}

// -----------------------------------------------------------------------------
const AuHeader& AccessUnit::GetHeader() const { return header; }

// -----------------------------------------------------------------------------
void AccessUnit::PrintDebug(std::ostream& output, uint8_t, uint8_t) const {
  output << "* Access Unit " << header.GetId() << " ";
  switch (header.GetClass()) {
    case core::record::ClassType::kNone:
      output << "INVALID";
      break;
    case core::record::ClassType::kClassU:
      output << "U";
      break;
    case core::record::ClassType::kClassP:
      output << "P";
      break;
    case core::record::ClassType::kClassN:
      output << "N";
      break;
    case core::record::ClassType::kClassM:
      output << "M";
      break;
    case core::record::ClassType::kClassI:
      output << "I";
      break;
    case core::record::ClassType::kClassHm:
      output << "HM";
      break;
  }
  if (header.GetClass() != core::record::ClassType::kClassU) {
    output << ", [" << header.GetAlignmentInfo().GetRefId() << "-"
           << header.GetAlignmentInfo().GetStartPos() << ":"
           << header.GetAlignmentInfo().GetEndPos() << "]";
  }

  output << ", " << header.GetReadCount() << " records";
  output << ", " << header.GetNumBlocks() << " blocks";
  output << ", parameter set " << static_cast<uint32_t>(header.GetParameterId())
         << std::endl;
}

// -----------------------------------------------------------------------------
AccessUnit::AccessUnit(AuHeader h, std::vector<Block> b)
    : DataUnit(DataUnitType::kAccessUnit),
      header(std::move(h)),
      blocks(std::move(b)),
      payloadbytes(0),
      qv_payloads(0) {}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

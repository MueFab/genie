/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/access_unit.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include "genie/core/parameter/parameter_set.h"
#include "genie/util/bitwriter.h"
#include "genie/util/data-block.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::debugPrint(const core::parameter::EncodingSet &ps) const {
    std::string lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
    std::cerr << "AU " << header.getID() << ": class " << lut[static_cast<int>(header.getClass())];
    if (header.getClass() != genie::core::record::ClassType::CLASS_U) {
        std::cerr << ", Position [" << header.getAlignmentInfo().getRefID() << "-"
                  << header.getAlignmentInfo().getStartPos() << ":" << header.getAlignmentInfo().getEndPos() << "]";
    }
    std::cerr << ", " << header.getReadCount() << " records";

    if (header.getClass() == genie::core::record::ClassType::CLASS_U) {
        if (!ps.isComputedReference()) {
            std::cerr << " (Low Latency)";
        } else {
            if (ps.getComputedRef().getAlgorithm() == core::parameter::ComputedRef::Algorithm::GLOBAL_ASSEMBLY) {
                std::cerr << " (Global Assembly)";
            } else {
                UTILS_DIE("Computed ref not supported: " +
                          std::to_string(static_cast<int>(ps.getComputedRef().getAlgorithm())));
            }
        }
    } else {
        if (!ps.isComputedReference()) {
            std::cerr << " (Reference)";
        } else {
            if (ps.getComputedRef().getAlgorithm() == core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY) {
                std::cerr << " (Local Assembly)";
            } else {
                UTILS_DIE("Computed ref not supported: " +
                          std::to_string(static_cast<int>(ps.getComputedRef().getAlgorithm())));
            }
        }
    }
    std::cerr << "..." << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::loadPayload(util::BitReader &bitReader) {
    for (size_t i = 0; i < header.getNumBlocks(); ++i) {
        blocks.emplace_back(qv_payloads, bitReader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::getPayloadSize() const { return payloadbytes; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(const std::map<size_t, core::parameter::EncodingSet> &parameterSets, util::BitReader &bitReader,
                       bool lazyPayload)
    : DataUnit(DataUnitType::ACCESS_UNIT) {
    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
    uint64_t bitreader_pos = bitReader.getBitsRead() / 8 - 1;
    bitReader.read_b(3);
    uint32_t du_size = bitReader.read<uint32_t>(29);

    header = AUHeader(bitReader, parameterSets);

    uint64_t bytesRead = (bitReader.getBitsRead() / 8 - bitreader_pos);
    payloadbytes = du_size - bytesRead;
    qv_payloads = parameterSets.at(header.getParameterID()).getQVConfig(header.getClass()).getNumSubsequences();

    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");

    if (!lazyPayload) {
        loadPayload(bitReader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
                       uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, bool signatureFlag)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      header(_access_unit_ID, _parameter_set_ID, _au_type, _reads_count, dataset_type, posSize, signatureFlag),
      blocks(0),
      payloadbytes(0),
      qv_payloads(0) {
    if (_au_type == core::record::ClassType::CLASS_N || _au_type == core::record::ClassType::CLASS_M) {
        header.setMmCfg(MmCfg());
    }
    if (dataset_type == DatasetType::REFERENCE) {
        header.setRefCfg(RefCfg(posSize));
    }
    if (_au_type != core::record::ClassType::CLASS_U) {
        header.setAuTypeCfg(AuTypeCfg(posSize));
    } else {
        if (signatureFlag) {
            header.setSignatureCfg(SignatureCfg());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::setMmCfg(MmCfg &&cfg) {
    if (!mm_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("MmCfg not valid for this access unit");
    }
    mm_cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::setRefCfg(RefCfg &&cfg) {
    if (!ref_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("RefCfg not valid for this access unit");
    }
    ref_cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

const RefCfg &AUHeader::getRefCfg() { return ref_cfg.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::setAuTypeCfg(AuTypeCfg &&cfg) {
    if (!au_Type_U_Cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("au_type_u_cfg not valid for this access unit");
    }
    au_Type_U_Cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::setSignatureCfg(SignatureCfg &&cfg) {
    if (!signature_config) {
        UTILS_THROW_RUNTIME_EXCEPTION("signature config not valid for this access unit");
    }
    signature_config = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Block> &AccessUnit::getBlocks() { return blocks; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AUHeader::getID() const { return access_unit_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AUHeader::getParameterID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const AuTypeCfg &AUHeader::getAlignmentInfo() const { return *au_Type_U_Cfg; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AUHeader::getReadCount() const { return reads_count; }

// ---------------------------------------------------------------------------------------------------------------------

core::record::ClassType AUHeader::getClass() const { return au_type; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::write(util::BitWriter &writer) const {
    DataUnit::write(writer);
    writer.write(0, 3);

    // Calculate size and write structure to tmp buffer
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    header.write(tmp_writer, true);
    tmp_writer.flush();
    uint64_t bits = tmp_writer.getBitsWritten();
    const uint64_t TYPE_SIZE_SIZE = 8 + 3 + 29;  // data_unit_type, reserved, data_unit_size
    bits += TYPE_SIZE_SIZE;
    uint64_t bytes = bits / 8;

    for (auto &i : blocks) {
        bytes += i.getWrittenSize();
    }

    // Now size is known, write to final destination
    writer.write(bytes, 29);
    writer.writeBypass(&ss);
    for (auto &i : blocks) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addBlock(Block block) {
    header.blockAdded();
    blocks.push_back(std::move(block));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

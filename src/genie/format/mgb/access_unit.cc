/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access_unit.h"
#include <genie/core/parameter/parameter_set.h>
#include <genie/util/bitwriter.h>
#include <genie/util/data-block.h>
#include <genie/util/runtime-exception.h>
#include <map>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::loadPayload(util::BitReader &bitReader) {
    for (size_t i = 0; i < num_blocks; ++i) {
        blocks.emplace_back(qv_payloads, bitReader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::getPayloadSize() const { return payloadbytes; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(const std::map<size_t, core::parameter::ParameterSet> &parameterSets, util::BitReader &bitReader,
                       bool lazyPayload)
    : DataUnit(DataUnitType::ACCESS_UNIT) {
    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
    uint64_t bitreader_pos = bitReader.getBitsRead() / 8 - 1;
    bitReader.read_b(3);
    uint32_t du_size = bitReader.read<uint32_t>(29);

    access_unit_ID = bitReader.read<uint32_t>();
    num_blocks = bitReader.read<uint8_t>();
    parameter_set_ID = bitReader.read<uint8_t>();
    au_type = bitReader.read<core::record::ClassType>(4);
    reads_count = bitReader.read<uint32_t>();
    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
        this->mm_cfg = MmCfg(bitReader);
    }

    if (parameterSets.at(parameter_set_ID).getDatasetType() == core::parameter::ParameterSet::DatasetType::REFERENCE) {
        this->ref_cfg = RefCfg(parameterSets.at(parameter_set_ID).getPosSize(), bitReader);
    }

    if (au_type != core::record::ClassType::CLASS_U) {
        this->au_Type_U_Cfg = AuTypeCfg(parameterSets.at(parameter_set_ID).getPosSize(),
                                        parameterSets.at(parameter_set_ID).hasMultipleAlignments(), bitReader);
    } else {
        this->signature_config = SignatureCfg(parameterSets.at(parameter_set_ID).getSignatureSize(),
                                              parameterSets.at(parameter_set_ID).getMultipleSignatureBase(), bitReader);
    }

    bitReader.flush();

    uint64_t bytesRead = (bitReader.getBitsRead() / 8 - bitreader_pos);
    payloadbytes = du_size - bytesRead;
    qv_payloads = parameterSets.at(parameter_set_ID).getQVConfig(au_type).getNumSubsequences();

    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");

    if (!lazyPayload) {
        loadPayload(bitReader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
                       uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
                       uint32_t multiple_signature_base)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      access_unit_ID(_access_unit_ID),
      num_blocks(0),
      parameter_set_ID(_parameter_set_ID),
      au_type(_au_type),
      reads_count(_reads_count),
      mm_cfg(),
      ref_cfg(),
      au_Type_U_Cfg(),
      signature_config(),
      blocks(0) {
    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
        mm_cfg = MmCfg();
    }
    if (dataset_type == DatasetType::REFERENCE) {
        ref_cfg = RefCfg(posSize);
    }
    if (au_type != core::record::ClassType::CLASS_U) {
        au_Type_U_Cfg = AuTypeCfg(posSize);
    } else {
        if (multiple_signature_base != 0) {
            signature_config = SignatureCfg(0, signatureSize);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setMmCfg(MmCfg &&cfg) {
    if (!mm_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("MmCfg not valid for this access unit");
    }
    mm_cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setRefCfg(RefCfg &&cfg) {
    if (!ref_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("RefCfg not valid for this access unit");
    }
    ref_cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

const RefCfg &AccessUnit::getRefCfg() { return ref_cfg.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setAuTypeCfg(AuTypeCfg &&cfg) {
    if (!au_Type_U_Cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("au_type_u_cfg not valid for this access unit");
    }
    au_Type_U_Cfg = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setSignatureCfg(SignatureCfg &&cfg) {
    if (!signature_config) {
        UTILS_THROW_RUNTIME_EXCEPTION("signature config not valid for this access unit");
    }
    signature_config = std::move(cfg);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Block> AccessUnit::getBlocks() const { return blocks; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AccessUnit::getID() const { return access_unit_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AccessUnit::getParameterID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const AuTypeCfg &AccessUnit::getAlignmentInfo() const { return *au_Type_U_Cfg; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AccessUnit::getReadCount() const { return reads_count; }

// ---------------------------------------------------------------------------------------------------------------------

core::record::ClassType AccessUnit::getClass() const { return au_type; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::write(util::BitWriter &writer) const {
    DataUnit::write(writer);
    writer.write(0, 3);

    // Calculate size and write structure to tmp buffer
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    preWrite(tmp_writer);
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

void AccessUnit::preWrite(util::BitWriter &writer) const {
    writer.write(access_unit_ID, 32);
    writer.write(num_blocks, 8);
    writer.write(parameter_set_ID, 8);
    writer.write(uint8_t(au_type), 4);
    writer.write(reads_count, 32);
    if (mm_cfg) {
        mm_cfg->write(writer);
    }
    if (ref_cfg) {
        ref_cfg->write(writer);
    }
    if (au_Type_U_Cfg) {
        au_Type_U_Cfg->write(writer);
    }
    if (signature_config) {
        signature_config->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addBlock(Block block) {
    ++num_blocks;
    blocks.push_back(std::move(block));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

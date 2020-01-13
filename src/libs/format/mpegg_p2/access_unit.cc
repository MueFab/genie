#include "access_unit.h"
#include "parameter_set.h"
#include "util/bitwriter.h"
#include "util/exceptions.h"

#include <gabac/data-block.h>
#include <map>
#include <sstream>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
AccessUnit::AccessUnit(const std::map<size_t, std::unique_ptr<format::mpegg_p2::ParameterSet>> &parameterSets,
                       util::BitReader &bitReader)
    : DataUnit(DataUnitType::ACCESS_UNIT) {
    reserved = bitReader.read(3);
    bitReader.read(29);

    access_unit_ID = bitReader.read(32);
    num_blocks = bitReader.read(8);
    parameter_set_ID = bitReader.read(8);
    au_type = mpegg_rec::ClassType(bitReader.read(4));
    reads_count = bitReader.read(32);
    if (au_type == mpegg_rec::ClassType::CLASS_N || au_type == mpegg_rec::ClassType::CLASS_M) {
        this->mm_cfg = util::make_unique<MmCfg>(bitReader);
    }

    if (parameterSets.at(parameter_set_ID)->getDatasetType() == ParameterSet::DatasetType::REFERENCE) {
        this->ref_cfg = util::make_unique<RefCfg>(parameterSets.at(parameter_set_ID)->getPosSize(), bitReader);
    }

    if (au_type != mpegg_rec::ClassType::CLASS_U) {
        this->au_Type_U_Cfg =
            util::make_unique<AuTypeCfg>(parameterSets.at(parameter_set_ID)->getPosSize(),
                                         parameterSets.at(parameter_set_ID)->hasMultipleAlignments(), bitReader);
    } else {
        this->signature_config =
            util::make_unique<SignatureCfg>(parameterSets.at(parameter_set_ID)->getSignatureSize(),
                                            parameterSets.at(parameter_set_ID)->getMultipleSignatureBase(), bitReader);
    }

    bitReader.flush();

    for (size_t i = 0; i < num_blocks; ++i) {
        blocks.emplace_back(bitReader);
    }
}

AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, mpegg_rec::ClassType _au_type,
                       uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, uint8_t signatureSize,
                       uint32_t multiple_signature_base)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      reserved(0),
      access_unit_ID(_access_unit_ID),
      num_blocks(0),
      parameter_set_ID(_parameter_set_ID),
      au_type(_au_type),
      reads_count(_reads_count),
      mm_cfg(nullptr),
      ref_cfg(nullptr),
      au_Type_U_Cfg(nullptr),
      signature_config(nullptr),
      blocks(0) {
    if (au_type == mpegg_rec::ClassType::CLASS_N || au_type == mpegg_rec::ClassType::CLASS_M) {
        mm_cfg = util::make_unique<MmCfg>();
    }
    if (dataset_type == DatasetType::REFERENCE) {
        ref_cfg = util::make_unique<RefCfg>(posSize);
    }
    if (au_type != mpegg_rec::ClassType::CLASS_U) {
        au_Type_U_Cfg = util::make_unique<AuTypeCfg>(posSize);
    } else {
        if (multiple_signature_base != 0) {
            signature_config = util::make_unique<SignatureCfg>(0, signatureSize);
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

void AccessUnit::setMmCfg(std::unique_ptr<MmCfg> cfg) {
    if (!mm_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("MmCfg not valid for this access unit");
    }
    mm_cfg = std::move(cfg);
}

void AccessUnit::setRefCfg(std::unique_ptr<RefCfg> cfg) {
    if (!ref_cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("RefCfg not valid for this access unit");
    }
    ref_cfg = std::move(cfg);
}

void AccessUnit::setAuTypeCfg(std::unique_ptr<AuTypeCfg> cfg) {
    if (!au_Type_U_Cfg) {
        UTILS_THROW_RUNTIME_EXCEPTION("au_type_u_cfg not valid for this access unit");
    }
    au_Type_U_Cfg = std::move(cfg);
}

void AccessUnit::setSignatureCfg(std::unique_ptr<SignatureCfg> cfg) {
    if (!signature_config) {
        UTILS_THROW_RUNTIME_EXCEPTION("signature config not valid for this access unit");
    }
    signature_config = std::move(cfg);
}

// -----------------------------------------------------------------------------------------------------------------
void AccessUnit::write(util::BitWriter &writer) const {
    DataUnit::write(writer);
    writer.write(reserved, 3);

    // Calculate size and write structure to tmp buffer
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    preWrite(tmp_writer);
    tmp_writer.flush();
    for (auto &i : blocks) {
        i.write(tmp_writer);
    }
    tmp_writer.flush();
    uint64_t bits = tmp_writer.getBitsWritten();
    const uint64_t TYPE_SIZE_SIZE = 8 + 3 + 29;  // data_unit_type, reserved, data_unit_size
    bits += TYPE_SIZE_SIZE;
    const uint64_t bytes = bits / 8;

    // Now size is known, write to final destination
    writer.write(bytes, 29);
    writer.write(&ss);
}

// -----------------------------------------------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------------------------------------------

void AccessUnit::addBlock(Block block) {
    ++num_blocks;
    blocks.push_back(std::move(block));
}
}  // namespace mpegg_p2
}  // namespace format
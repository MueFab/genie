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

bool AUHeader::operator==(const AUHeader &other) const {
    return access_unit_ID == other.access_unit_ID && num_blocks == other.num_blocks &&
           parameter_set_ID == other.parameter_set_ID && au_type == other.au_type && reads_count == other.reads_count &&
           mm_cfg == other.mm_cfg && ref_cfg == other.ref_cfg && au_Type_U_Cfg == other.au_Type_U_Cfg &&
           signature_config == other.signature_config;
}

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::write(genie::util::BitWriter &writer, bool write_signatures) const {
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
    if (write_signatures) {
        if (au_Type_U_Cfg) {
            au_Type_U_Cfg->write(writer);
        }
        if (signature_config) {
            signature_config->write(writer);
        }
    }
    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AUHeader::getNumBlocks() const { return num_blocks; }

// ---------------------------------------------------------------------------------------------------------------------

void AUHeader::blockAdded() { num_blocks++; }

// ---------------------------------------------------------------------------------------------------------------------

AUHeader::AUHeader(util::BitReader &bitReader, const std::map<size_t, core::parameter::EncodingSet> &parameterSets,
                   bool read_signatures) {
    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
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
    if (read_signatures) {
        if (au_type != core::record::ClassType::CLASS_U) {
            this->au_Type_U_Cfg = AuTypeCfg(parameterSets.at(parameter_set_ID).getPosSize(),
                                            parameterSets.at(parameter_set_ID).hasMultipleAlignments(), bitReader);
        } else {
            if (parameterSets.at(parameter_set_ID).isSignatureActivated()) {
                uint8_t length = 0;
                if (parameterSets.at(parameter_set_ID).isSignatureConstLength()) {
                    length = parameterSets.at(parameter_set_ID).getSignatureConstLength();
                }
                this->signature_config =
                    SignatureCfg(bitReader, length,
                                 genie::core::getAlphabetProperties(parameterSets.at(parameter_set_ID).getAlphabetID()).base_bits);
            }
        }
    }
    bitReader.flush();
    UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
}

// ---------------------------------------------------------------------------------------------------------------------

AUHeader::AUHeader()
    : AUHeader(0, 0, core::record::ClassType::NONE, 0, core::parameter::ParameterSet::DatasetType::NON_ALIGNED, 0,
               false, core::AlphabetID::ACGTN) {}

// ---------------------------------------------------------------------------------------------------------------------

AUHeader::AUHeader(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
                   uint32_t _reads_count, genie::core::parameter::ParameterSet::DatasetType dataset_type,
                   uint8_t posSize, bool signatureFlag, core::AlphabetID alphabet)
    : access_unit_ID(_access_unit_ID),
      num_blocks(0),
      parameter_set_ID(_parameter_set_ID),
      au_type(_au_type),
      reads_count(_reads_count),
      mm_cfg(),
      ref_cfg(),
      au_Type_U_Cfg(),
      signature_config() {
    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
        mm_cfg = MmCfg();
    }
    if (dataset_type == genie::core::parameter::ParameterSet::DatasetType::REFERENCE) {
        ref_cfg = RefCfg(posSize);
    }
    if (au_type != core::record::ClassType::CLASS_U) {
        au_Type_U_Cfg = AuTypeCfg(posSize);
    } else {
        if (signatureFlag) {
            signature_config = SignatureCfg(genie::core::getAlphabetProperties(alphabet).base_bits);
        }
    }
}

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
                       uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, bool signatureFlag,
                       core::AlphabetID alphabet)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      header(_access_unit_ID, _parameter_set_ID, _au_type, _reads_count, dataset_type, posSize, signatureFlag,
             alphabet),
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
            header.setSignatureCfg(SignatureCfg(core::getAlphabetProperties(alphabet).base_bits));
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

AUHeader &AccessUnit::getHeader() { return header; }

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

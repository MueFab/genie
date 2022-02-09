/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/access_unit_header.h"

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
                this->signature_config = SignatureCfg(
                    bitReader, length,
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

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

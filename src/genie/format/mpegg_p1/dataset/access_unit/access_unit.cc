/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <sstream>

#include "access_unit.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
                                 uint32_t _reads_count, core::parameter::DataUnit::DatasetType dataset_type,
                                 uint8_t posSize, uint8_t signatureSize, uint32_t multiple_signature_base)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      access_unit_ID(_access_unit_ID),
      num_blocks(0),
      parameter_set_ID(_parameter_set_ID),
      au_type(_au_type),
      reads_count(_reads_count) {

    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
        mm_cfg = util::make_unique<mgb::MmCfg>();
    }
    if (dataset_type == DatasetType::REFERENCE) {
        ref_cfg = util::make_unique<mgb::RefCfg>(posSize);
    }
    if (au_type != core::record::ClassType::CLASS_U) {
        au_Type_U_Cfg = util::make_unique<mgb::AuTypeCfg>(posSize);
    } else {
        if (multiple_signature_base != 0) {
            signature_config = util::make_unique<mgb::SignatureCfg>(0, signatureSize);
        }
    }
}

AccessUnit::AccessUnit(mgb::AccessUnit&& au)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      access_unit_ID(au.getID()),
      num_blocks(0),
      parameter_set_ID(au.getParameterID()),
      au_type(au.getClass()),
      reads_count(au.getReadCount()) {
}

uint64_t AccessUnit::getHeaderLength() const {
    // Calculate size and write structure to tmp buffer
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    writeHeader(tmp_writer, true);
    tmp_writer.flush();

    return tmp_writer.getBitsWritten() / 8;
}

void AccessUnit::writeHeader(util::BitWriter& writer, bool write_dummy_length) const {
    // Key of KVL format
    writer.write("aucn");

    if (write_dummy_length){
        // Length of KVL format
        writer.write(0, 64);
    } else {
        // Length of KVL format
        writer.write(getHeaderLength(), 64);
    }

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

uint64_t AccessUnit::getLength() const {
    uint64_t len = 12;  // KVL

    len += getHeaderLength();

    if (AU_information){
        len += AU_information->getLength();
    }

    if (AU_protection){
        len += AU_protection->getLength();
    }

    if (block_header_flag){
        for (auto& block: blocks){
            len += block.getLength();
        }
    }

    return len;
}


void AccessUnit::write(util::BitWriter& writer) const {
    // Key of KVL format
    writer.write("aucn");

    // Length of KVL format
    writer.write(getLength(), 64);

    writeHeader(writer);

    if (AU_information){
        AU_information->write(writer);
    }

    if (AU_protection){
        AU_protection->write(writer);
    }

    if (block_header_flag){
        for (auto& block: blocks){
            block.write(writer);
        }
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
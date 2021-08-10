/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/access_unit/access_unit_header.h"
#include <sstream>

namespace genie {
namespace format {
namespace mpegg_p1 {

AccessUnitHeader::AccessUnitHeader()
    : ID(0),
      num_blocks(0),
      parameter_set_ID(0),
      au_type(core::record::ClassType::NONE),
      reads_count(0),
      mm_cfg(nullptr),
      ref_cfg(nullptr),
      au_type_cfg(nullptr),
      signature_cfgs() {}


AccessUnitHeader::AccessUnitHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                                   DatasetHeader& dhd)
    : signature_cfgs(){

    MIT_flag = dhd.getMITFlag();

    /// access_unit_ID u(32)
    ID = reader.read<uint32_t>();

    /// num_blocks u(8)
    num_blocks = reader.read<uint8_t>();

    /// parameter_set_ID u(8)
    parameter_set_ID = reader.read<uint8_t>();

    /// au_type u(4)
    au_type = reader.read<core::record::ClassType>(4);

    /// reads_count u(32)
    reads_count = reader.read<uint32_t>();

    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M){
        mm_cfg = util::make_unique<format::mgb::MmCfg>(reader);
    } else {
        mm_cfg = nullptr;
    }

    /// dataset_type == 2
    if (dhd.getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE){
        auto pos_size = (uint64_t)dhd.getPos40SizeFlag();
        ref_cfg = util::make_unique<format::mgb::RefCfg>(pos_size,
                                                         reader);
    } else {
        ref_cfg = nullptr;
    }

    if (!MIT_flag){
        if (au_type != core::record::ClassType::CLASS_U){
            au_type_cfg = util::make_unique<format::mgb::AuTypeCfg>((uint64_t)dhd.getPos40SizeFlag(),
                                                                    dhd.getMultipleAlignmentFlag(),
                                                                    reader);

        } else {
            au_type_cfg = nullptr;

            if (dhd.getUAccessUnitInfo().getUSignatureFlag()){
                auto num_signatures = reader.read<uint16_t>();
                for (auto i=0; i<num_signatures; i++){
                    auto signature_cfg = util::make_unique<SignatureCfg>(reader, dhd);
                    signature_cfgs.emplace_back(std::move(signature_cfg));
                }
            }
        }
    }

    reader.flush();
#if ROUNDTRIP_CONSTRUCTOR
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    tmp_writer.flush();
    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
    UTILS_DIE_IF(wlen != length, "Invalid AccessUnitHeader write()");
    UTILS_DIE_IF( elen != length, "Invalid AccessUnitHeader getLength()");
#endif

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid AccessUnitHeader length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t AccessUnitHeader::getID() const{ return ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AccessUnitHeader::getNumBlocks() const{return num_blocks;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitHeader::getLength() const{
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer, true);
    tmp_writer.flush();
    return tmp_writer.getBitsWritten() / 8;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitHeader::write(util::BitWriter& writer, bool zero_length) const{
    // Key of KVL format
    writer.write("auhd");

    /// Length of KLV format
    if (zero_length){
        writer.write(0, 64);
    } else {
        writer.write(getLength(), 64);
    }

    /// access_unit_ID u(32)
    writer.write(ID, 32);

    /// num_blocks u(8)
    writer.write(num_blocks, 8);

    /// parameter_set_ID u(8)
    writer.write(parameter_set_ID, 8);

    /// au_type u(4)
    writer.write((uint8_t)au_type, 4);

    /// reads_count u(32)
    writer.write(reads_count, 32);

    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M){
        mm_cfg->write(writer);
    } else {

    }

    /// dataset_type == 2
    if (ref_cfg != nullptr){
        ref_cfg->write(writer);
    } else {

    }

    if (!MIT_flag){
        if (au_type_cfg != nullptr){
            au_type_cfg->write(writer);
        } else {
            for (auto i=0; i<signature_cfgs.size();i++){
                signature_cfgs[i]->write(writer);
            }
        }
    }

    writer.flush();
}

}
}
}
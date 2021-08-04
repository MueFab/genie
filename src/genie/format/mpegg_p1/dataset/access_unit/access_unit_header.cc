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
      signature_cfg(nullptr) {}


AccessUnitHeader::AccessUnitHeader(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                                   DatasetHeader& dhd){

//    dataset_type = dhd.getDatasetType();
////    MIT_flag = dhd.getMITFlag();
//    alphabet_ID = dhd.getAlphabetID();
//    multiple_alignment_flag = dhd.getMultipleAlignmentFlag();

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

//    dataset_type = dhd.getDatasetType();
    /// dataset_type == 2
    if (dhd.getDatasetType() == core::parameter::DataUnit::DatasetType::REFERENCE){
        ref_cfg = util::make_unique<format::mgb::RefCfg>((uint64_t)dhd.getPos40SizeFlag(),
                                                         reader);
    } else {
        ref_cfg = nullptr;
    }

    if (dhd.getMITFlag()){
        if (au_type != core::record::ClassType::CLASS_U){
            au_type_cfg = util::make_unique<format::mgb::AuTypeCfg>((uint64_t)dhd.getPos40SizeFlag(),
                                                                    dhd.getMultipleAlignmentFlag(),
                                                                    reader);
            signature_cfg = nullptr;
        } else {
            au_type_cfg = nullptr;
            signature_cfg = util::make_unique<SignatureCfg>(reader, dhd);
        }
    }

    reader.flush();
//#if ROUNDTRIP_CONSTRUCTOR
//    std::stringstream ss;
//    util::BitWriter tmp_writer(&ss);
//    write(tmp_writer);
//    tmp_writer.flush();
//    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
//    uint64_t elen = getLength();
//    UTILS_DIE_IF(wlen != length, "Invalid DatasetHeader write()");
//    UTILS_DIE_IF( elen != length, "Invalid DatasetHeader getLength()");
//#endif

    auto x = reader.getPos() - start_pos;
    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid AccessUnitHeader length!");
}


}
}
}
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/access_unit/access_unit.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

//AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
//                       uint32_t _reads_count, core::parameter::DataUnit::DatasetType dataset_type, uint8_t posSize,
//                       uint8_t signatureSize, uint32_t multiple_signature_base)
//    : DataUnit(DataUnitType::ACCESS_UNIT),
//      access_unit_ID(_access_unit_ID),
//      num_blocks(0),
//      parameter_set_ID(_parameter_set_ID),
//      au_type(_au_type),
//      reads_count(_reads_count) {
//    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
//        mm_cfg = util::make_unique<mgb::MmCfg>();
//    }
//    if (dataset_type == DatasetType::REFERENCE) {
//        ref_cfg = util::make_unique<mgb::RefCfg>(posSize);
//    }
//    if (au_type != core::record::ClassType::CLASS_U) {
//        au_Type_U_Cfg = util::make_unique<mgb::AuTypeCfg>(posSize);
//    } else {
//        if (multiple_signature_base != 0) {
//            signature_config = util::make_unique<mgb::SignatureCfg>(0, signatureSize);
//        }
//    }
//}
//
//// ---------------------------------------------------------------------------------------------------------------------
//
//AccessUnit::AccessUnit(mgb::AccessUnit&& au)
//    : DataUnit(DataUnitType::ACCESS_UNIT),
//      access_unit_ID(au.getID()),
//      num_blocks(0),
//      parameter_set_ID(au.getParameterID()),
//      au_type(au.getClass()),
//      reads_count(au.getReadCount()) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                       DatasetHeader& dhd) {

    /// access_unit_header
    size_t box_start_pos = reader.getPos();
    std::string box_key = readKey(reader);
    auto box_length = reader.read<uint64_t>();
    UTILS_DIE_IF(box_key != "auhd",
                 "Access unit header is not found!");
    header = AccessUnitHeader(reader, fhd, box_start_pos, box_length, dhd);

    do {
        /// Read K,L of KLV
        box_start_pos = reader.getPos();
        box_key = readKey(reader);
        box_length = reader.read<uint64_t>();

//        /// reference[]
//        if (box_key == "rfgn") {
//            references.emplace_back(reader, fhd, box_start_pos, box_length);
//
//            /// reference_metadata[]
//        } else if (box_key == "rfmd") {
//            //TODO(Yeremia) Insert reference metadata here
//            UTILS_DIE("TODO");
//
//            /// label_list
//        } else if (box_key == "labl") {
//            label_list = util::make_unique<LabelList>(reader, box_length);
//
//            /// DG_metadata
//        } else if (box_key == "dgmd") {
//            DG_metadata = util::make_unique<DGMetadata>(reader, box_length);
//
//            /// DG_protection
//        } else if (box_key == "dgpr") {
//            DG_protection = util::make_unique<DGProtection>(reader, box_length);
//
//            /// Dataset
//        } else if (box_key == "dtcn") {
//            datasets.emplace_back(reader, fhd, box_start_pos, box_length);
//        }
    } while (reader.getPos() - start_pos < length);

    UTILS_DIE_IF(!reader.isAligned() || (reader.getPos() - start_pos != length), "Invalid DatasetGroup length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getLength() const {
//    uint64_t len = 12;  // KVL
//
//    len += getHeaderLength();
//
//    // TODO(Yeremia): Integration of part 3
////    if (AU_information) {
////        len += AU_information->getLength();
////    }
////
////    if (AU_protection) {
////        len += AU_protection->getLength();
////    }
//
//    if (block_header_flag) {
//        for (auto& block : blocks) {
//            len += block.getLength();
//        }
//    }
//
//    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::write(util::BitWriter& writer) const {
    // Key of KVL format
    writer.write("aucn");

    // Length of KVL format
    writer.write(getLength(), 64);

//    writeHeader(writer);
//
//    // TODO(Yeremia): Integration of part 3
////    if (AU_information) {
////        AU_information->write(writer);
////    }
////
////    if (AU_protection) {
////        AU_protection->write(writer);
////    }
//
//    if (block_header_flag) {
//        for (auto& block : blocks) {
//            block.write(writer);
//        }
//    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

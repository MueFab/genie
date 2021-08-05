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

AUInformation::AUInformation(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length)
    : minor_version(fhd.getMinorVersion()){

    if (minor_version != "1900"){
        dataset_group_ID = reader.read<uint8_t>();
        dataset_ID = reader.read<uint16_t>();
    }
    for (auto ibyte=0; ibyte<length-(4+8);ibyte++){
        AU_information_value.emplace_back(reader.read<uint8_t>());
    }
#if ROUNDTRIP_CONSTRUCTOR
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    tmp_writer.flush();
    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
    UTILS_DIE_IF(wlen != length, "Invalid AUInformation write()");
    UTILS_DIE_IF( elen != length, "Invalid AUInformation getLength()");
#endif
    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid AUInformation length!");
}

uint64_t AUInformation::getLength() const{
    return AU_information_value.size();
}
void AUInformation::write(genie::util::BitWriter& writer) const{
    writer.write("auin");

    writer.write(getLength(), 64);

    if (minor_version != "1900"){
        writer.write(dataset_group_ID, 8);
        writer.write(dataset_ID, 16);
    }

    for (auto& byte:AU_information_value){
        writer.write(byte, 8);
    }
}

AUProtection::AUProtection(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length)
    : minor_version(fhd.getMinorVersion()){

    if (minor_version != "1900"){
        dataset_group_ID = reader.read<uint8_t>();
        dataset_ID = reader.read<uint16_t>();
    }

    for (auto ibyte=0; ibyte<length-(4+8);ibyte++){
        AU_protection_value.emplace_back(reader.read<uint8_t>());
    }
#if ROUNDTRIP_CONSTRUCTOR
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    tmp_writer.flush();
    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
    UTILS_DIE_IF(wlen != length, "Invalid AUProtection write()");
    UTILS_DIE_IF( elen != length, "Invalid AUProtection getLength()");
#endif
    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid AUProtection length!");
}
uint64_t AUProtection::getLength() const{
    return AU_protection_value.size();
}
void AUProtection::write(genie::util::BitWriter& writer) const{
    writer.write("aupr");

    writer.write(getLength(), 64);

    if (minor_version != "1900"){
        writer.write(dataset_group_ID, 8);
        writer.write(dataset_ID, 16);
    }

    for (auto& byte:AU_protection_value){
        writer.write(byte, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length,
                       DatasetHeader& dhd)
    : block_header_flag(dhd.getBlockHeaderFlag()){

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

        /// AU_information
        if (box_key == "auin") {
            UTILS_DIE_IF(AU_information != nullptr, "Multiple AU_information found within one access unit!");
            AU_information = util::make_unique<AUInformation>(reader, fhd, box_start_pos, box_length);
        /// AU_protection
        } else if (box_key == "aupr") {
            UTILS_DIE_IF(AU_protection != nullptr, "Multiple AU_protection found within one access unit!");
            AU_protection = util::make_unique<AUProtection>(reader, fhd, box_start_pos, box_length);

        /// Handle block[i]
        } else {

            /// This needs to be done as blocks does not have Key
            reader.flush();
            reader.setPos(box_start_pos);

            if (block_header_flag){
                for (auto i=0; i<header.getNumBlocks(); i++){
                    blocks.emplace_back(reader, fhd);
                }
            }
        }

    } while (reader.getPos() - start_pos < length);

#if ROUNDTRIP_CONSTRUCTOR
//    std::stringstream ss;
//    util::BitWriter tmp_writer(&ss);
//    write(tmp_writer);
//    tmp_writer.flush();
//    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
//    UTILS_DIE_IF(wlen != length, "Invalid AccessUnitHeader write()");
    UTILS_DIE_IF( elen != length, "Invalid AccessUnit getLength()");
#endif
    UTILS_DIE_IF(!reader.isAligned() || (reader.getPos() - start_pos != length),
                 "Invalid AccessUnit length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getLength() const {
    uint64_t len = 12;  /// K,L
    len += header.getLength();

    if (AU_information != nullptr) {
        len += AU_information->getLength();
    }

    /// AU_information
    if (AU_protection != nullptr) {
        len += AU_protection->getLength();
    }

    if (block_header_flag) {
        for (auto& block : blocks) {
            len += block.getLength();
        }
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::write(util::BitWriter& writer) const {
    /// Key of KVL format
    writer.write("aucn");

    /// Length of KVL format
    writer.write(getLength(), 64);

    /// AccessUnitHeader
    header.write(writer);

    /// AU_information
    if (AU_information) {
        AU_information->write(writer);
    }

    /// AU_protection
    if (AU_protection) {
        AU_protection->write(writer);
    }

    if (block_header_flag) {
        for (auto& block : blocks) {
            block.write(writer);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

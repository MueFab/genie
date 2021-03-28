/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata()
    : DT_metadata_value() {}

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata(std::vector<uint8_t>&& _DT_metadata_value)
    : DT_metadata_value(std::move(_DT_metadata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata(util::BitReader& reader, size_t length) {

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dtmd", "DTMetadata is not Found");

    size_t start_pos = reader.getPos();

    // DT_metadata_value[uint8_t]
    for (auto& val : DT_metadata_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DTMetadata length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DTMetadata::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // DT_metadata_value[] std::vector<uint8_t>
    len += DT_metadata_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTMetadata::write(genie::util::BitWriter &bit_writer) const {

    /// KLV (Key Length Value) format
    // Key of KLV format
    bit_writer.write("dtmd");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // DT_metadata_value[] std::vector<uint8_t>
    for (auto val : DT_metadata_value){
        bit_writer.write(val,8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection()
    : DT_protection_value() {}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection(std::vector<uint8_t>&& _DT_protection_value)
    : DT_protection_value(std::move(_DT_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection(util::BitReader& reader, size_t length)
    : DT_protection_value() {

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dtpr", "DTProtection is not Found");

    size_t start_pos = reader.getPos();

    // DT_protection_value[uint8_t]
    for (auto& val : DT_protection_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DTProtection length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DTProtection::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    // DT_protection_value[] std::vector<uint8_t>
    len += DT_protection_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTProtection::write(genie::util::BitWriter &bit_writer) const {
    /// // KLV (Key Length Value) format
    // Key of KVL format
    bit_writer.write("dtpr");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // DT_protection_value[] std::vector<uint8_t>
    for (auto val : DT_protection_value){
        bit_writer.write(val, 8);
    }
}
// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(uint16_t _dataset_ID)
    :header(_dataset_ID)
{}

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(uint8_t group_ID, uint16_t ID, DatasetHeader::ByteOffsetSizeFlag _byte_offset_size_flag,
                 bool _non_overlapping_AU_range_flag,
                 DatasetHeader::Pos40SizeFlag _pos_40_bits_flag,
                 bool _multiple_alignment_flag,
                 core::parameter::DataUnit::DatasetType _dataset_type,
                 uint8_t _alphabet_ID,
                 uint32_t _num_U_access_units,
                 std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2,
                 const genie::format::mgb::DataUnitFactory& dataUnitFactory
//                 std::unique_ptr<MasterIndexTable> _master_index_table
)

    : header(group_ID, ID, _byte_offset_size_flag, _non_overlapping_AU_range_flag, _pos_40_bits_flag,
             _multiple_alignment_flag, _dataset_type, _alphabet_ID, _num_U_access_units)
//    master_index_table(std::move(_master_index_table))

{
    /// dataset_parameter_set[]
    for (unsigned int i = 0;; ++i) {
        try {  // to iterate over dataUnitFactory.getParams(i)
            dataset_parameter_sets.emplace_back(ID, std::move(dataUnitFactory.getParams(i)));
        } catch (const std::out_of_range&) {
            // std::cout << "Got " << i << " ParameterSet/s from DataUnitFactory" << std::endl; //debuginfo
            break;
        }
    }

    /// access_unit[]
    for (auto& au : accessUnits_p2) {
        access_units.emplace_back(std::move(au));
    }

    /// descriptor_stream[]
    if (header.getBlockHeader().getBlockHeaderFlag() == 0) {
        for (auto& ds : descriptor_streams) {
            descriptor_streams.emplace_back(std::move(ds));
        }
    }
}
// ---------------------------------------------------------------------------------------------------------------------
/*
Dataset::Dataset(util::BitReader& reader, size_t length) {

    size_t start_pos = reader.getPos();

    std::string key = readKey(reader, "XXXX");
    UTILS_DIE_IF(key != "dtcn", "DatasetHeader is not Found");

    /// Class Dataset_header
    auto header_length = reader.read<size_t>();
    header = DatasetHeader(reader, header_length);

    do {
        key = readKey(reader, "XXXX");
        auto metadata_length = reader.read<size_t>();
        if (key == "dtmd"){
            DT_metadata = util::make_unique<DatasetHeader>(reader, metadata_length);
        } else if (key == "dtpr"){
            DT_metadata = util::make_unique<DatasetHeader>(reader, protection_length);
        } else if (key == "pars"){

        } else if (key == "mitb"){

        } else if (key == "aucun"){

        } else if (key == "dscn"){

        }

    } while (reader.getPos() - start_pos < length);




    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid Dataset length!");
}
*/

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DatasetParameterSet>& Dataset::getParameterSets() const { return dataset_parameter_sets; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetHeader& Dataset::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

uint16_t Dataset::getID() const { return getHeader().getID(); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setID(uint16_t ID) { header.setID(ID); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Dataset::getGroupID() const { return getHeader().getGroupID(); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setGroupId(uint8_t group_ID){ header.setGroupId(group_ID);}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::ByteOffsetSizeFlag Dataset::getByteOffsetSizeFlag() const { return getHeader().getByteOffsetSizeFlag();}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::Pos40SizeFlag Dataset::getPos40SizeFlag() const { return getHeader().getPos40SizeFlag();}

// ---------------------------------------------------------------------------------------------------------------------

const SequenceConfig& Dataset::getSeqInfo() const { return getHeader().getSeqInfo();}

// ---------------------------------------------------------------------------------------------------------------------

const BlockConfig& Dataset::getBlockHeader() const { return getHeader().getBlockHeader();}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Dataset::getNumUAccessUnits() const { return getHeader().getNumUAccessUnits();}

// ---------------------------------------------------------------------------------------------------------------------

bool Dataset::getMultipleAlignmentFlag() const { return getHeader().getMultipleAlignmentFlag();}

// ---------------------------------------------------------------------------------------------------------------------

core::parameter::DataUnit::DatasetType Dataset::getDatasetType() const { return getHeader().getDatasetType();}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Dataset::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    len += header.getLength();

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->getLength();
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->getLength();
    }

    for (auto const& it : dataset_parameter_sets) {
        // dataset_group_ID u(8) + dataset_ID u(16)
        len += sizeof(uint8_t) + sizeof(uint16_t);
        len += it.getLength();
    }

    // TODO: Master Index Table
    //write master_index_table depending on MIT_FLAG
    //if (getBlockHeader().getMITFlag()){
    //    if (master_index_table != nullptr) {
    //        master_index_table->getLength();
    //    }
    //}


    for (auto const& it : access_units) {
        len += it.getLength();
    }

    // TODO (Yeremia): write descriptor_stream depending on block_header_flag
    //if (getBlockHeader().getBlockHeaderFlag() == 0){
    //    for (auto &ds: descriptor_streams){
    //        len += ds.getLength();
    //    }
    //}

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::write(util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("dtcn");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // Value of KLV format:

    // dataset_header
    header.write(bit_writer);

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->write(bit_writer);
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->write(bit_writer);
    }

    // TODO: Master Index Table
    // write master_index_table depending on MIT_FLAG
    //if (getBlockHeader().getMITFlag()){
    //    master_index_table->write(bit_writer);
    //}

    // dataset_parameter_set[]
    for (auto const& ps : dataset_parameter_sets) {
        ps.write(bit_writer);
    }

    for (auto &ac: access_units){
        ac.write(bit_writer);
    }

    // TODO (Yeremia): write descriptor_stream depending on block_header_flag
    //if (getBlockHeader().getBlockHeaderFlag() == 0){
    //    for (auto &ds: descriptor_streams){
    //        ds.write(bit_writer);
    //    }
    //}

    // TODO (Yeremia): implement write of Dataset
    UTILS_DIE("Not implemented yet");
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
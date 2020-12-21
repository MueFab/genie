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

uint64_t DTMetadata::getLength() const {
    uint64_t len = 12;  // gen_info

    len += DT_metadata_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTMetadata::write(genie::util::BitWriter &bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("dtmd");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    for (auto val : DT_metadata_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DTProtection::getLength() const {
    uint64_t len = 12;  // gen_info

    len += DT_protection_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTProtection::write(genie::util::BitWriter &bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("dtpr");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    for (auto val : DT_protection_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//Dataset::Dataset(uint16_t ID, const genie::format::mgb::DataUnitFactory& dataUnitFactory,
//                 std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2)
//    : dataset_group_ID(group_ID),
//      dataset_ID(ID),
//      version("XXXX"), // TODO (Yeremia): Version
//      byte_offset_size_flag(_byte_offset_size_flag),
//      non_overlapping_AU_range_flag(_non_overlapping_AU_range_flag),
//      pos_40_bits_flag(_pos_40_bits_flag),
//      dataset_type(_dataset_type),
//      alphabet_ID(_alphabet_ID),
//      num_U_access_units(_num_U_access_units){
//
//    for (unsigned int i = 0;; ++i) {
//        try {  // to iterate over dataUnitFactory.getParams(i)
//            dataset_parameter_sets.emplace_back(ID, std::move(dataUnitFactory.getParams(i)));
//        } catch (const std::out_of_range&) {
//            // std::cout << "Got " << i << " ParameterSet/s from DataUnitFactory" << std::endl; //debuginfo
//            break;
//        }
//    }
//
//    for (auto& au : accessUnits_p2) {
//        access_units.emplace_back(std::move(au));
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DatasetParameterSet>& Dataset::getParameterSets() const { return dataset_parameter_sets; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Dataset::getLength() const {
    uint64_t len = 12;  // KVL

    len += getHeaderLength();

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->getLength();
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->getLength();
    }

    // write master_index_table depending on MIT_FLAG
    if (block_config.getMITFlag()){
        master_index_table->getLength();
    }

    for (auto const& it : dataset_parameter_sets) {
        // dataset_group_ID u(8) + dataset_ID u(16)
        len += sizeof(uint8_t) + sizeof(uint16_t);
        len += it.getLength();
    }

    for (auto const& it : access_units) {
        len += it.getLength();
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::write(util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("dtcn");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // Value of KVL format:

    // dataset_header
    writeHeader(bit_writer);

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->write(bit_writer);
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->write(bit_writer);
    }

    // write master_index_table depending on MIT_FLAG
    if (block_config.getMITFlag()){
        master_index_table->write(bit_writer);
    }

    // dataset_parameter_set[]
    for (auto const& ps : dataset_parameter_sets) {
        ps.write(bit_writer);
    }

    for (auto &ac: access_units){
        ac.write(bit_writer);
    }

    // TODO (Yeremia): write descriptor_stream depending on block_header_flag
//    if (block_header_flag == 0){
//        for (auto &ds: descriptor_streams){
//            dc.write(bit_writer);
//        }
//    }

    // TODO (Yeremia): implement write of Dataset
    UTILS_DIE("Not implemented yet");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
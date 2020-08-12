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
    // TODO (Yeremia): length of Value[]?

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
    // TODO (Yeremia): length of Value[]?

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

Dataset::Dataset(uint16_t dataset_ID, const genie::format::mgb::DataUnitFactory& dataUnitFactory,
                 std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2)
    : dataset_header(dataset_ID) {  // TODO: dataset_header constructor

    for (unsigned int i = 0;; ++i) {
        try {  // to iterate over dataUnitFactory.getParams(i)
            dataset_parameter_sets.emplace_back(dataset_ID, std::move(dataUnitFactory.getParams(i)));
        } catch (const std::out_of_range&) {
            // std::cout << "Got " << i << " ParameterSet/s from DataUnitFactory" << std::endl; //debuginfo
            break;
        }
    }

    for (auto& au : accessUnits_p2) {
        access_units.emplace_back(std::move(au));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Dataset::getID() const { return dataset_header.getID(); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setDatasetGroupId(uint8_t _dataset_group_ID) {

    dataset_header.setDatasetGroupId(_dataset_group_ID);

    for (auto& ps : dataset_parameter_sets) {
        ps.setDatasetGroupId(_dataset_group_ID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Dataset::getDatasetParameterSetDatasetID() const {
    return dataset_parameter_sets.front().getDatasetID();
}  // only returns ID of first ps in vector

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Dataset::getDatasetParameterSetDatasetGroupID() const {
    return dataset_parameter_sets.front().getDatasetGroupID();
}  // only returns ID of first ps in vector

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DatasetParameterSet>& Dataset::getDatasetParameterSets() const { return dataset_parameter_sets; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetHeader& Dataset::getDatasetHeader() const { return dataset_header; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Dataset::getLength() const {
    uint64_t len = 12;  // gen_info
    // TODO (Yeremia): length of Value[]?

    len += dataset_header.getLength();

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->getLength();
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->getLength();
    }

    for (auto const& it : dataset_parameter_sets) {
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

    // dataset_header DatasetHeader
    dataset_header.write(bit_writer);

    // DT_metadata
    if (DT_metadata != nullptr){
        DT_metadata->write(bit_writer);
    }

    // DT_protection
    if (DT_protection != nullptr){
        DT_protection->write(bit_writer);
    }

    // TODO (Yeremia): write master_index_table depending on MIT_FLAG
//    if (MIT_FLAG){
//        master_index_table.write(bit_writer);
//    }

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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
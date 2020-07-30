/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Dataset::Dataset(uint16_t dataset_ID)
    : dataset_header(dataset_ID){}  // TODO: dataset_header constructor

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(const genie::format::mgb::DataUnitFactory& dataUnitFactory,
                 std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2, const uint16_t dataset_ID)
    : dataset_header(dataset_ID) {  // TODO: dataset_header constructor

    for (unsigned int i = 0;; ++i) {
        try {  // to iterate over dataUnitFactory.getParams(i)
            dataset_parameter_sets.emplace_back(std::move(dataUnitFactory.getParams(i)), dataset_ID);
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
    uint64_t length = 12;  // gen_info
    length += dataset_header.getLength();

    for (auto const& it : dataset_parameter_sets) {
        length += it.getLength();
    }

    for (auto const& it : access_units) {
        length += it.getLength();
    }

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::writeToFile(util::BitWriter& bit_writer) const {

    //TODO (Yeremia): is it required?
    bit_writer.write("dtcn");

    //TODO (Yeremia): is it required?
    bit_writer.write(this->getLength(), 64);

    // dataset_header DatasetHeader
    dataset_header.write(bit_writer);

    // TODO (Yeremia): Write DG_metadata
    //DG_metadata.writeToFile(bit_writer);

    // TODO (Yeremia): Write DG_protection
    //DG_protection.writeToFile(bit_writer);

    // TODO (Yeremia): writeToFile master_index_table depending on MIT_FLAG
//    if (MIT_FLAG){
//        master_index_table.writeToFile(bit_writer);
//    }

    for (auto &ac: access_units){
        ac.writeToFile(bit_writer);
    }

    // TODO (Yeremia): writeToFile descriptor_stream depending on block_header_flag
//    if (block_header_flag == 0){
//        for (auto &ds: descriptor_streams){
//            dc.writeToFile(bit_writer);
//        }
//    }

    // TODO (Yeremia): implement writeToFile of Dataset
    UTILS_DIE("Not implemented yet");
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
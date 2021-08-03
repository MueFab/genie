/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

DatasetGroupHeader::DatasetGroupHeader():
    ID(0), version(0),
    dataset_IDs() {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(genie::util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length):
    ID(reader.read<uint8_t>()), version(reader.read<uint8_t>()),
    dataset_IDs()
{
    auto num_datasets = (length - 14)/2;
    for (auto i=0; i<num_datasets;i++){
        dataset_IDs.push_back(reader.read<uint16_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getID() const{ return ID;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::setID(uint8_t _ID) { ID = _ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getVersion() const{ return version;}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint16_t>& DatasetGroupHeader::getDatasetIDs(){ return dataset_IDs;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getLength() const{
    /// dataset_group_ID u(8), version u(8), dataset_IDs[] u(16)
    return 1 + 1 + 2*dataset_IDs.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::write(genie::util::BitWriter& writer) const{
    /// dataset_group_ID u(8)
    writer.write(ID, 8);

    /// version u(8)
    writer.write(ID, 8);

    /// dataset_IDs[] u(16)
    for (auto& dataset_ID: dataset_IDs){
        writer.write(dataset_ID, 16);
    }
}

}
}
}
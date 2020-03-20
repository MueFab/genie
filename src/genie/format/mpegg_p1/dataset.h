/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_H
#define GENIE_DATASET_H

#include <memory>
#include <vector>

#include "access_unit_p1.h"
#include "dataset_header.h"
#include "dataset_parameter_set.h"
#include "genie/format/mgb/data-unit-factory.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DT_metadata {
   public:
    DT_metadata() : DT_metadata_value() {
        DT_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                             0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                             0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DT_metadata_value;
};

class DT_protection {
   public:
    DT_protection() : DT_protection_value() {
        DT_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                               0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                               0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DT_protection_value;
};

class Dataset {
   public:
    Dataset(const genie::format::mgb::DataUnitFactory& dataUnitFactory,
            std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2, const uint16_t dataset_ID);

    uint16_t getDatasetParameterSetDatasetID() const {
        return dataset_parameter_sets.front().getDatasetID();
    }  // only returns ID of first ps in vector

    uint8_t getDatasetParameterSetDatasetGroupID() const {
        return dataset_parameter_sets.front().getDatasetGroupID();
    }  // only returns ID of first ps in vector

    const DatasetHeader& getDatasetHeader() const { return dataset_header; }
    const std::vector<DatasetParameterSet>& getDatasetParameterSets() const { return dataset_parameter_sets; }

    const uint64_t getLength() const;

    void setDatasetHeaderGroupId(uint8_t GroupId) { dataset_header.setDatasetGroupId(GroupId); }
    void setDatasetParameterSetsGroupId(uint8_t GroupId);

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 18
     *
     * ------------------------------------------------------------------------------------------------------------- */
    mpegg_p1::DatasetHeader dataset_header;
    // std::unique_ptr<DT_metadata> dt_metadata;     //optional
    // std::unique_ptr<DT_protection> dt_protection; //optional
    std::vector<DatasetParameterSet> dataset_parameter_sets;
    /** master_index_table is optional and not yet implemented */
    std::vector<mpegg_p1::AccessUnit> access_units;
    /** descriptor_stream[] is optional and not yet implemented */
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_DATASET_H

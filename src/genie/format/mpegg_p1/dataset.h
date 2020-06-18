/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_H
#define GENIE_DATASET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "access_unit_p1.h"
#include "dataset_header.h"
#include "dataset_parameter_set.h"
#include "genie/format/mgb/data-unit-factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DT_metadata {
   public:
    /**
     *
     */
    DT_metadata();

   private:
    std::vector<uint8_t> DT_metadata_value;  //!<
};

/**
 *
 */
class DT_protection {
   public:
    /**
     *
     */
    DT_protection();

   private:
    std::vector<uint8_t> DT_protection_value;  //!<
};

/**
 *
 */
class Dataset {
   public:
    /**
     *
     * @param dataUnitFactory
     * @param accessUnits_p2
     * @param dataset_ID
     */
    Dataset(const genie::format::mgb::DataUnitFactory& dataUnitFactory,
            std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2, const uint16_t dataset_ID);

    /**
     *
     * @return
     */
    uint16_t getDatasetParameterSetDatasetID() const;

    /**
     *
     * @return
     */
    uint8_t getDatasetParameterSetDatasetGroupID() const;

    /**
     *
     * @return
     */
    const DatasetHeader& getDatasetHeader() const;

    /**
     *
     * @return
     */
    const std::vector<DatasetParameterSet>& getDatasetParameterSets() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param groupId
     */
    void setDatasetGroupId(uint8_t groupId);

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    /**
     *
     */
    void setDatasetHeaderGroupId(uint8_t groupId);

    /**
     *
     * @param groupId
     */
    void setDatasetParameterSetsGroupId(uint8_t groupId);

    mpegg_p1::DatasetHeader dataset_header;  //!<
    // std::unique_ptr<DT_metadata> dt_metadata;     //optional
    // std::unique_ptr<DT_protection> dt_protection; //optional
    std::vector<DatasetParameterSet> dataset_parameter_sets;  //!<
    /** master_index_table is optional and not yet implemented */
    std::vector<mpegg_p1::AccessUnit> access_units;  //!<
    /** descriptor_stream[] is optional and not yet implemented */
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATASET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_MPEGG_P1_DATASET_DATASET
#define GENIE_FORMAT_MPEGG_P1_DATASET_DATASET

#include <memory>
#include <vector>

#include <genie/core/parameter/data_unit.h>
#include <genie/core/parameter/parameter_set.h>
#include <genie/format/mgb/data-unit-factory.h>

//#include <genie/format/mpegg_p1/dataset/master_index_table/master_index_table.h>
#include <genie/format/mpegg_p1/dataset/access_unit/access_unit.h>
#include <genie/format/mpegg_p1/dataset/dataset_parameter_set.h>

#include "dataset_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DTMetadata {
   public:
    /**
     *
     */
    DTMetadata();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

   private:
    std::vector<uint8_t> DT_metadata_value;  //!<
};

/**
 *
 */
class DTProtection {
   public:
    /**
     *
     */
    DTProtection();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

   private:
    std::vector<uint8_t> DT_protection_value;  //!<
};

class Dataset {
   private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2.2 table 18
     *  ------------------------------------------------------------------------------------------------------------ */

    DatasetHeader header;

    // ISO 23092-1 Section 6.5.2.3 - specification 23092-3
    // optional
    std::unique_ptr<DTMetadata> DT_metadata;

    // ISO 23092-1 Section 6.5.2.4 - specification 23092-3
    // optional
    std::unique_ptr<DTProtection> DT_protection;

    // ISO 23092-1 Section 6.5.2.5 - specification 23092-2
    // optional
    std::vector<DatasetParameterSet> dataset_parameter_sets;

    // ISO 23092-1 Section 6.5.3.1 - specification 23092-1
    // optional
    // TODO: Master Index Table
//    std::unique_ptr<MasterIndexTable> master_index_table;

    // ISO 23092-1 Section 6.5.3
    std::vector<AccessUnit> access_units;

    // TODO(Yeremia): Descriptor Stream
    // ISO 23092-1 Section 6.5.4
//    std::vector<DescriptorStream> descriptor_streams;

   public:
    /**
     *
     * @param _dataset_ID
     */
    Dataset(uint16_t _dataset_ID);

    /**
     *
     * @param dataUnitFactory
     * @param accessUnits_p2
     * @param ID
     */
    Dataset(uint16_t ID, const genie::format::mgb::DataUnitFactory& dataUnitFactory,
            std::vector<genie::format::mgb::AccessUnit>& accessUnits_p2);

    uint16_t getID() const;

    void setID(uint16_t ID);

    uint8_t getGroupID() const;

    void setGroupId(uint8_t group_ID);

    /**
     *
     * @return
     */
    const std::vector<DatasetParameterSet>& getParameterSets() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_FORMAT_MPEGG_P1_DATASET_DATASET

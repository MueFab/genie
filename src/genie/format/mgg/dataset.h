/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "genie/core/meta/block_header/disabled.h"
#include "genie/core/meta/block_header/enabled.h"
#include "genie/core/meta/dataset.h"
#include "genie/format/mgb/mgb_file.h"
#include "genie/format/mgg/access_unit.h"
#include "genie/format/mgg/dataset_header.h"
#include "genie/format/mgg/dataset_metadata.h"
#include "genie/format/mgg/dataset_parameter_set.h"
#include "genie/format/mgg/dataset_protection.h"
#include "genie/format/mgg/descriptor_stream.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table.h"

#define GENIE_DEBUG_PRINT_NODETAIL

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class Dataset : public GenInfo {
 private:
    DatasetHeader header;                                  //!< @brief
    boost::optional<DatasetMetadata> metadata;             //!< @brief
    boost::optional<DatasetProtection> protection;         //!< @brief
    std::vector<DatasetParameterSet> parameterSets;        //!< @brief
    boost::optional<MasterIndexTable> master_index_table;  //!< @brief
    std::vector<AccessUnit> access_units;                  //!< @brief
    std::vector<DescriptorStream> descriptor_streams;      //!< @brief
    core::MPEGMinorVersion version;                        //!< @brief

    std::map<size_t, core::parameter::EncodingSet> encoding_sets;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    bool hasMetadata() const;

    /**
     * @brief
     * @return
     */
    bool hasProtection() const;

    /**
     * @brief
     * @return
     */
    DatasetMetadata& getMetadata();

    /**
     * @brief
     * @return
     */
    DatasetProtection& getProtection();

    /**
     * @brief
     * @return
     */
    std::vector<AccessUnit>& getAccessUnits();

    /**
     * @brief
     * @return
     */
    std::vector<DescriptorStream>& getDescriptorStreams();

    /**
     * @brief
     * @return
     */
    std::vector<DatasetParameterSet>& getParameterSets();

    /**
     * @brief
     * @param reader
     * @param _version
     */
    Dataset(util::BitReader& reader, core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param file
     * @param meta
     * @param _version
     * @param param_ids
     */
    Dataset(format::mgb::MgbFile& file, core::meta::Dataset& meta, core::MPEGMinorVersion _version,
            const std::vector<uint8_t>& param_ids);

    /**
     * @brief
     * @param groupID
     * @param setID
     */
    void patchID(uint8_t groupID, uint16_t setID);

    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief
     * @return
     */
    const DatasetHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    DatasetHeader& getHeader();

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param reader
     * @param in_offset
     */
    void read_box(util::BitReader& reader, bool in_offset);

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

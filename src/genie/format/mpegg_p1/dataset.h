/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/access_unit.h"
#include "genie/format/mpegg_p1/dataset_header.h"
#include "genie/format/mpegg_p1/dataset_metadata.h"
#include "genie/format/mpegg_p1/dataset_parameter_set.h"
#include "genie/format/mpegg_p1/dataset_protection.h"
#include "genie/format/mpegg_p1/descriptor_stream.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/master_index_table.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
     * @param reader
     * @param _version
     */
    Dataset(util::BitReader& reader, core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    const DatasetHeader& getHeader() const;

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

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Dataset");
        header.print_debug(output, depth + 1, max_depth);
        if (metadata) {
            metadata->print_debug(output, depth + 1, max_depth);
        }
        if (protection) {
            metadata->print_debug(output, depth + 1, max_depth);
        }
        for(const auto& r : parameterSets) {
            r.print_debug(output, depth + 1, max_depth);
        }
        if(master_index_table) {
            master_index_table->print_debug(output, depth + 1, max_depth);
        }
        for(const auto& r : access_units) {
            r.print_debug(output, depth + 1, max_depth);
        }
        for(const auto& r : descriptor_streams) {
            r.print_debug(output, depth + 1, max_depth);
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

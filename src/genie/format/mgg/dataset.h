/**
 * @file
 * @brief This file defines the `Dataset` class, which represents a dataset in MPEG-G format.
 * @details The `Dataset` class encapsulates various components such as metadata, protection, parameter sets,
 *          descriptor streams, and access units. It supports serialization and deserialization, along with
 *          several utility methods for modifying and retrieving dataset information. This class is integral
 *          for managing dataset structure and ensuring compatibility with the MPEG-G standard.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "genie/core/meta/blockheader/disabled.h"
#include "genie/core/meta/blockheader/enabled.h"
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
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

#define GENIE_DEBUG_PRINT_NODETAIL

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a dataset in MPEG-G format.
 * @details The `Dataset` class encapsulates all the data related to a dataset, such as metadata, protection,
 *          parameter sets, and descriptor streams. It allows for modifying and accessing these components,
 *          as well as serialization and deserialization to and from bitstreams.
 */
class Dataset : public GenInfo {
 private:
    DatasetHeader header;                                //!< @brief Dataset header containing core information.
    std::optional<DatasetMetadata> metadata;             //!< @brief Optional dataset metadata.
    std::optional<DatasetProtection> protection;         //!< @brief Optional dataset protection information.
    std::vector<DatasetParameterSet> parameterSets;      //!< @brief List of parameter sets used in the dataset.
    std::optional<MasterIndexTable> master_index_table;  //!< @brief Optional master index table for the dataset.
    std::vector<AccessUnit> access_units;                //!< @brief List of access units within the dataset.
    std::vector<DescriptorStream> descriptor_streams;    //!< @brief List of descriptor streams.
    core::MPEGMinorVersion version;                      //!< @brief MPEG minor version for the dataset.

    std::map<size_t, core::parameter::EncodingSet>
        encoding_sets;  //!< @brief Map of encoding sets based on parameter IDs.

 public:
    /**
     * @brief Checks if the dataset has metadata.
     * @return True if metadata is present; otherwise, false.
     */
    [[nodiscard]] bool hasMetadata() const;

    /**
     * @brief Checks if the dataset has protection information.
     * @return True if protection is present; otherwise, false.
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief Retrieves the metadata object.
     * @return Reference to the `DatasetMetadata` object.
     */
    DatasetMetadata& getMetadata();

    /**
     * @brief Retrieves the protection object.
     * @return Reference to the `DatasetProtection` object.
     */
    DatasetProtection& getProtection();

    /**
     * @brief Retrieves the list of access units.
     * @return Reference to the vector of access units.
     */
    std::vector<AccessUnit>& getAccessUnits();

    /**
     * @brief Retrieves the list of descriptor streams.
     * @return Reference to the vector of descriptor streams.
     */
    std::vector<DescriptorStream>& getDescriptorStreams();

    /**
     * @brief Retrieves the list of parameter sets.
     * @return Reference to the vector of parameter sets.
     */
    std::vector<DatasetParameterSet>& getParameterSets();

    /**
     * @brief Constructs a `Dataset` object by reading from a bit reader.
     * @param reader Bit reader to extract dataset data from.
     * @param _version MPEG minor version to use for the dataset.
     */
    Dataset(util::BitReader& reader, core::MPEGMinorVersion _version);

    /**
     * @brief Constructs a `Dataset` object from an existing `MgbFile` and metadata.
     * @param file Reference to the `MgbFile` object.
     * @param meta Metadata of the dataset.
     * @param _version MPEG minor version to use for the dataset.
     * @param param_ids List of parameter set IDs.
     */
    Dataset(format::mgb::MgbFile& file, core::meta::Dataset& meta, core::MPEGMinorVersion _version,
            const std::vector<uint8_t>& param_ids);

    /**
     * @brief Patches the group ID and set ID within the dataset.
     * @param groupID New group ID.
     * @param setID New dataset ID.
     */
    void patchID(uint8_t groupID, uint16_t setID);

    /**
     * @brief Patches the reference ID within the dataset.
     * @param _old Old reference ID to replace.
     * @param _new New reference ID to set.
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief Retrieves the header of the dataset.
     * @return Constant reference to the `DatasetHeader` object.
     */
    [[nodiscard]] const DatasetHeader& getHeader() const;

    /**
     * @brief Retrieves the header of the dataset.
     * @return Reference to the `DatasetHeader` object.
     */
    DatasetHeader& getHeader();

    /**
     * @brief Serializes the dataset into a bit writer.
     * @param bitWriter Bit writer to write the dataset to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the unique key for the dataset.
     * @return The key as a string.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Reads and initializes the dataset from a bit reader.
     * @param reader Bit reader to read the dataset from.
     * @param in_offset Indicates whether to account for bit offsets.
     */
    void read_box(util::BitReader& reader, bool in_offset);

    /**
     * @brief Prints detailed debugging information for the dataset.
     * @param output Output stream to print to.
     * @param depth Current depth of the debug output.
     * @param max_depth Maximum depth to display.
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

}  // namespace genie::format::mgg

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

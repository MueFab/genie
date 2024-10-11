/**
 * @file
 * @brief This file defines the `DataSetMappingTable` class, representing a mapping table for datasets in the MPEG-G format.
 * @details The `DataSetMappingTable` class provides a mechanism for associating multiple data streams with a single dataset.
 *          It stores references to these data streams and provides methods for adding, accessing, and serializing/deserializing
 *          the data. Each `DataSetMappingTable` is identified by a unique dataset ID and contains a list of `DataStream` objects.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mgg/data_stream.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class for managing a mapping table for datasets in MPEG-G format.
 * @details The `DataSetMappingTable` class stores a set of data streams associated with a dataset ID.
 *          It provides functionalities to add new streams, retrieve the dataset ID, and handle bitstream serialization
 *          and deserialization. This class is used to describe the structure and organization of data streams within a dataset.
 */
class DataSetMappingTable : public GenInfo {
 public:
    /**
     * @brief Retrieves the unique key associated with this dataset mapping table.
     * @details The key is used to uniquely identify this mapping table within a dataset group.
     * @return A constant reference to the key string.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Serializes the dataset mapping table to a `BitWriter`.
     * @param bitWriter The bit writer to output the mapping table.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the dataset ID associated with this mapping table.
     * @return The ID of the dataset.
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief Retrieves the list of data streams associated with this dataset mapping table.
     * @return A constant reference to the list of data streams.
     */
    [[nodiscard]] const std::vector<DataStream>& getDataStreams() const;

    /**
     * @brief Adds a new data stream to the dataset mapping table.
     * @param d The data stream to be added.
     */
    void addDataStream(DataStream d);

    /**
     * @brief Constructs a new `DataSetMappingTable` object with a given dataset ID.
     * @param _dataset_id The ID of the dataset this mapping table represents.
     */
    explicit DataSetMappingTable(uint16_t _dataset_id);

    /**
     * @brief Constructs a new `DataSetMappingTable` object by reading from a `BitReader`.
     * @param reader The bit reader to extract data from.
     */
    explicit DataSetMappingTable(util::BitReader& reader);

    /**
     * @brief Compares this `DataSetMappingTable` object with another `GenInfo` object for equality.
     * @param info The `GenInfo` object to compare against.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

 private:
    uint16_t dataset_id;              //!< @brief ID of the dataset this mapping table represents.
    std::vector<DataStream> streams;  //!< @brief List of data streams associated with this dataset.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_MAPPING_TABLE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

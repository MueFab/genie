/**
 * @file
 * @brief This file defines the `DescriptorStream` class, which handles encapsulating a descriptor stream and its
 * metadata in an MPEG-G format.
 * @details The `DescriptorStream` class is responsible for storing the header information, optional protection, and
 * payload data related to a descriptor stream in an MPEG-G file. It provides methods for reading and writing this data,
 * as well as accessing and manipulating the stream's metadata.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <optional>
#include <string>
#include <vector>
#include "genie/format/mgg/block.h"
#include "genie/format/mgg/descriptor_stream_header.h"
#include "genie/format/mgg/descriptor_stream_protection.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table.h"
#include "genie/util/bit-reader.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a descriptor stream, encapsulating its metadata and payloads.
 * @details This class is used to encapsulate a descriptor stream, which includes a `DescriptorStreamHeader`, optional
 *          `DescriptorStreamProtection`, and a set of payloads. The payloads represent the actual data blocks
 *          associated with the descriptor. This class provides functionality for reading, writing, and manipulating the
 * descriptor stream.
 */
class DescriptorStream : public GenInfo {
 private:
    DescriptorStreamHeader header;  //!< @brief Header information for the descriptor stream.
    std::optional<DescriptorStreamProtection>
        ds_protection;                   //!< @brief Optional protection information for the stream.
    std::vector<core::Payload> payload;  //!< @brief Vector storing the payload data of the descriptor stream.

 public:
    /**
     * @brief Compares the current `DescriptorStream` object with another `GenInfo` object for equality.
     * @param info The `GenInfo` object to compare against.
     * @return True if both objects are equal; otherwise, false.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Constructs a `DescriptorStream` with a given header.
     * @param _header The `DescriptorStreamHeader` to initialize the stream with.
     */
    explicit DescriptorStream(DescriptorStreamHeader _header);

    /**
     * @brief Constructs a `DescriptorStream` from descriptor type and blocks.
     * @param descriptor The descriptor type for the stream.
     * @param classID The class type of the stream.
     * @param blocks Vector of blocks to initialize the payload with.
     */
    DescriptorStream(genie::core::GenDesc descriptor, genie::core::record::ClassType classID,
                     std::vector<format::mgb::Block> blocks);

    /**
     * @brief Decapsulates the descriptor stream into its constituent blocks.
     * @details Extracts and returns the blocks that make up the descriptor stream.
     * @return A vector of `format::mgb::Block` representing the decapsulated blocks.
     */
    std::vector<format::mgb::Block> decapsulate();

    /**
     * @brief Checks if the descriptor stream is empty.
     * @details This method returns true if there are no payloads present in the stream.
     * @return True if the stream is empty; otherwise, false.
     */
    [[nodiscard]] bool isEmpty() const;

    /**
     * @brief Constructs a `DescriptorStream` by reading from a bit reader.
     * @param reader The `BitReader` to read the descriptor stream from.
     * @param table Reference to the `MasterIndexTable` for decoding.
     * @param configs Vector of class configurations for processing the stream.
     */
    explicit DescriptorStream(util::BitReader& reader, const MasterIndexTable& table,
                              const std::vector<dataset_header::MITClassConfig>& configs);

    /**
     * @brief Retrieves the header of the descriptor stream.
     * @details This method returns a constant reference to the descriptor stream's header.
     * @return A constant reference to the `DescriptorStreamHeader` object.
     */
    [[nodiscard]] const DescriptorStreamHeader& getHeader() const;

    /**
     * @brief Adds a new payload to the descriptor stream.
     * @param p The `core::Payload` to be added.
     */
    void addPayload(core::Payload p);

    /**
     * @brief Retrieves the payloads stored in the descriptor stream.
     * @details This method returns a constant reference to the vector of payloads.
     * @return A constant reference to a vector of `core::Payload` objects.
     */
    [[nodiscard]] const std::vector<core::Payload>& getPayloads() const;

    /**
     * @brief Sets the optional protection for the descriptor stream.
     * @param prot The `DescriptorStreamProtection` object to set.
     */
    void setProtection(DescriptorStreamProtection prot);

    /**
     * @brief Retrieves the protection information of the descriptor stream.
     * @details This method returns a constant reference to the protection object.
     * @return A constant reference to the `DescriptorStreamProtection` object.
     */
    [[nodiscard]] const DescriptorStreamProtection& getProtection() const;

    /**
     * @brief Retrieves the protection information of the descriptor stream.
     * @details This method returns a modifiable reference to the protection object, allowing modifications.
     * @return A reference to the `DescriptorStreamProtection` object.
     */
    DescriptorStreamProtection& getProtection();

    /**
     * @brief Checks if the descriptor stream has protection information set.
     * @details This method returns true if protection information is present.
     * @return True if the stream has protection information; otherwise, false.
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief Serializes the descriptor stream to a `BitWriter`.
     * @details Writes the descriptor stream's metadata and payloads to the specified `BitWriter` object.
     * @param writer The `BitWriter` to write the serialized data to.
     */
    void box_write(util::BitWriter& writer) const override;

    /**
     * @brief Retrieves the unique key identifying the `DescriptorStream` object.
     * @details The key is used to identify this type of metadata box within an MPEG-G file.
     * @return A constant reference to the string representing the key identifier.
     */
    [[nodiscard]] const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

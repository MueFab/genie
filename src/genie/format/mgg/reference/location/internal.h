/**
 * @file
 * @brief Defines the `Internal` class for representing internal reference locations in MPEG-G files.
 *
 * The `Internal` class is a specialized type of `Location` used to represent references to sequences located within the
 * same MPEG-G file. It references other datasets within the MPEG-G container using identifiers for the dataset group
 * and the specific dataset. This allows MPEG-G files to internally link to sequences without requiring external
 * dependencies. The class also provides functionality for serialization and deserialization of the reference data.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/meta/refbase.h"
#include "genie/format/mgg/reference/location.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location {

/**
 * @brief Represents an internal reference location in an MPEG-G file.
 *
 * The `Internal` class extends the `Location` class and represents a reference to a sequence located in another dataset
 * within the same MPEG-G container. It uses a dataset group ID and a dataset ID to identify the location of the
 * reference sequence. This allows for efficient referencing within a single MPEG-G file, enabling self-contained
 * references without external dependencies.
 */
class Internal : public Location {
 private:
    uint8_t internal_dataset_group_id;  //!< @brief ID of the dataset group containing the reference.
    uint16_t internal_dataset_id;       //!< @brief ID of the specific dataset containing the reference.

 public:
    /**
     * @brief Constructs an `Internal` reference with specified parameters.
     *
     * @param _reserved Reserved byte for additional options.
     * @param _internal_dataset_group_id ID of the dataset group containing the reference.
     * @param _internal_dataset_id ID of the specific dataset containing the reference.
     */
    Internal(uint8_t _reserved, uint8_t _internal_dataset_group_id, uint16_t _internal_dataset_id);

    /**
     * @brief Constructs an `Internal` reference by reading from a bitstream.
     *
     * This constructor reads the data for the internal reference from a bitstream using the provided `BitReader`.
     *
     * @param reader The `BitReader` to read the reference data from.
     */
    explicit Internal(genie::util::BitReader& reader);

    /**
     * @brief Constructs an `Internal` reference by reading from a bitstream with a reserved byte.
     *
     * This constructor reads the data for the internal reference from a bitstream using the provided `BitReader`.
     *
     * @param reader The `BitReader` to read the reference data from.
     * @param _reserved Reserved byte value.
     */
    Internal(genie::util::BitReader& reader, uint8_t _reserved);

    /**
     * @brief Gets the dataset group ID of the internal reference.
     * @return The ID of the dataset group containing the reference.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Gets the dataset ID of the internal reference.
     * @return The ID of the specific dataset containing the reference.
     */
    [[nodiscard]] uint16_t getDatasetID() const;

    /**
     * @brief Serializes the `Internal` reference to a bitstream.
     *
     * This method serializes the internal reference's attributes into a bitstream for storage or transmission.
     *
     * @param writer The `BitWriter` to serialize the data to.
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief Converts the internal reference into a `RefBase` object.
     *
     * This method decapsulates the internal reference, converting it into a `RefBase` object that can be used
     * by higher-level MPEG-G metadata structures.
     *
     * @return A unique pointer to the decapsulated `RefBase` object.
     */
    std::unique_ptr<genie::core::meta::RefBase> decapsulate() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_INTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

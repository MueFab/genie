/**
 * @file reference_metadata.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `ReferenceMetadata` class for handling reference metadata within the MGG (MPEG-G) container format in GENIE.
 *
 * This header file specifies the implementation of the `ReferenceMetadata` class, which is used to manage
 * reference-related metadata information within the MGG (MPEG-G) container format of the GENIE framework.
 * The class provides methods to handle ID patching, data encapsulation, and serialization of metadata.
 *
 * The MGG container format is part of the MPEG-G (ISO/IEC 23092) standard, which is designed for efficient
 * representation, compression, and transport of genomic information. `ReferenceMetadata` encapsulates information
 * specific to a genomic dataset, enabling storage and retrieval of reference details within an MPEG-G file.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit-reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class ReferenceMetadata
 * @brief Manages reference metadata information within the MGG (MPEG-G) container format.
 *
 * The `ReferenceMetadata` class is a specialized implementation of the `GenInfo` base class,
 * designed to handle the encapsulation, serialization, and modification of reference metadata
 * in the MGG container, which is part of the MPEG-G standard.
 *
 * The MPEG-G standard provides a suite of technologies for efficient compression and transport of genomic data,
 * making it possible to store large-scale genomic information efficiently. The `ReferenceMetadata` class plays
 * a key role in this by storing metadata details for each genomic reference in an MGG container.
 */
class ReferenceMetadata : public GenInfo {
 private:
    uint8_t dataset_group_id;              //!< @brief ID of the associated dataset group.
    uint8_t reference_id;                  //!< @brief ID of the reference sequence.
    std::string reference_metadata_value;  //!< @brief Metadata value associated with the reference.

 public:
    /**
     * @brief Updates the dataset group ID for this metadata entry.
     * @param groupID New dataset group ID.
     */
    void patchID(uint8_t groupID);

    /**
     * @brief Patches the reference ID, replacing the old value with a new one.
     * @param _old Previous reference ID.
     * @param _new New reference ID.
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief Extracts the encapsulated metadata value.
     * @return The reference metadata value as a string.
     */
    std::string decapsulate();

    /**
     * @brief Compares two `GenInfo` objects for equality.
     * @param info The other `GenInfo` object to compare with.
     * @return True if the two objects are equal, false otherwise.
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief Returns a string key representing the type of metadata.
     * @return The key as a string.
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief Constructs a `ReferenceMetadata` object.
     * @param _dataset_group_id ID of the dataset group.
     * @param _reference_id ID of the reference sequence.
     * @param _reference_metadata_value Metadata value associated with the reference.
     */
    ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id, std::string _reference_metadata_value);

    /**
     * @brief Constructs a `ReferenceMetadata` object from a bitstream.
     * @param bitreader A `BitReader` object to read the metadata from.
     */
    explicit ReferenceMetadata(genie::util::BitReader& bitreader);

    /**
     * @brief Writes the reference metadata to a bitstream.
     * @param bitWriter The `BitWriter` object to write to.
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief Retrieves the dataset group ID.
     * @return The dataset group ID as a uint8_t.
     */
    [[nodiscard]] uint8_t getDatasetGroupID() const;

    /**
     * @brief Retrieves the reference ID.
     * @return The reference ID as a uint8_t.
     */
    [[nodiscard]] uint8_t getReferenceID() const;

    /**
     * @brief Retrieves the metadata value associated with the reference.
     * @return The reference metadata value as a string.
     */
    [[nodiscard]] const std::string& getReferenceMetadataValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_METADATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

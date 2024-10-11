/**
 * @file
 * @brief Defines the `ReferenceOptions` class used within the MPEG-G dataset header.
 *
 * The `ReferenceOptions` class encapsulates information related to reference sequences used
 * in a dataset. It keeps track of the reference ID, sequence IDs, and the corresponding sequence
 * block counts. This class provides utilities for reading, writing, and manipulating reference
 * sequence metadata within the MPEG-G format.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @class ReferenceOptions
 * @brief Represents reference sequence options within the MPEG-G dataset header.
 *
 * The `ReferenceOptions` class stores information about reference sequences associated
 * with a dataset. This includes the reference ID, the sequence IDs that the reference is linked to,
 * and the number of blocks in each sequence. It provides methods for reading, writing, and
 * updating the reference metadata.
 */
class ReferenceOptions {
 private:
    uint8_t reference_ID;              //!< @brief ID representing the reference sequence.
    std::vector<uint16_t> seq_ID;      //!< @brief List of sequence IDs associated with this reference.
    std::vector<uint32_t> seq_blocks;  //!< @brief Number of blocks corresponding to each sequence ID.

 public:
    /**
     * @brief Updates the reference ID, replacing an old ID with a new one.
     *
     * This method allows modifying the reference ID by replacing the old value with the new one.
     * It is useful when patching or reassigning reference IDs within the MPEG-G structure.
     *
     * @param _old The old reference ID to be replaced.
     * @param _new The new reference ID to use.
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief Checks for equality between two `ReferenceOptions` objects.
     *
     * Compares the reference ID, sequence IDs, and sequence blocks of this instance with another
     * `ReferenceOptions` object to determine if they are equal.
     *
     * @param other The other `ReferenceOptions` instance to compare with.
     * @return `true` if both instances have the same reference ID, sequence IDs, and blocks, `false` otherwise.
     */
    bool operator==(const ReferenceOptions& other) const;

    /**
     * @brief Constructs a `ReferenceOptions` object by reading from a bitstream.
     *
     * Reads the reference ID, sequence IDs, and sequence blocks from the given bitstream,
     * initializing the internal state of the `ReferenceOptions` instance.
     *
     * @param reader Bitstream reader used to extract the reference and sequence information.
     */
    explicit ReferenceOptions(genie::util::BitReader& reader);

    /**
     * @brief Serializes the `ReferenceOptions` to a bitstream.
     *
     * Writes the reference ID, sequence IDs, and sequence block counts into the specified bitstream writer.
     * This method is used when exporting the `ReferenceOptions` to an MPEG-G compatible format.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Default constructor for creating an empty `ReferenceOptions` object.
     *
     * Initializes the object with no reference ID, sequence IDs, or blocks. This constructor is
     * typically used when building an object incrementally.
     */
    ReferenceOptions();

    /**
     * @brief Adds a new sequence to the reference options.
     *
     * Appends a sequence ID and the corresponding block count to the internal vectors,
     * associating the sequence with the specified reference ID.
     *
     * @param _reference_ID The reference ID for the sequence.
     * @param _seq_id The sequence ID to add.
     * @param blocks The number of blocks in the sequence.
     */
    void addSeq(uint8_t _reference_ID, uint16_t _seq_id, uint32_t blocks);

    /**
     * @brief Retrieves the list of sequence IDs.
     *
     * Provides access to the internal vector of sequence IDs associated with this reference.
     *
     * @return Constant reference to the vector of sequence IDs.
     */
    [[nodiscard]] const std::vector<uint16_t>& getSeqIDs() const;

    /**
     * @brief Retrieves the list of sequence block counts.
     *
     * Provides access to the internal vector of block counts for each sequence ID.
     *
     * @return Constant reference to the vector of sequence block counts.
     */
    [[nodiscard]] const std::vector<uint32_t>& getSeqBlocks() const;

    /**
     * @brief Retrieves the reference ID.
     *
     * Returns the reference ID associated with these options, indicating which reference
     * sequence these options apply to.
     *
     * @return The reference ID.
     */
    [[nodiscard]] uint8_t getReferenceID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_REFERENCE_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

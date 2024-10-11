/**
 * @file
 * @brief Defines the `Offset` class used in MPEG-G data structures.
 *
 * The `Offset` class represents a key-value pair where the key is a subkey string and the value is a numerical offset.
 * It is used within MPEG-G containers to denote positions within the data streams, helping to keep track of the locations
 * of specific elements for faster access and manipulation.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_OFFSET_H_
#define SRC_GENIE_FORMAT_MGG_OFFSET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class Offset
 * @brief Represents an MPEG-G offset entry.
 *
 * This class encapsulates a mapping between a subkey string and a 64-bit offset value.
 * Offsets are used within MPEG-G containers to denote byte positions or other references,
 * enabling efficient data retrieval and manipulation. This class allows for reading from
 * and writing to bitstreams.
 */
class Offset {
 private:
    std::string subkey;  //!< @brief Subkey identifying the specific entry in the MPEG-G structure.
    uint64_t offset;     //!< @brief Offset value corresponding to the subkey.

 public:
    /**
     * @brief Constructs an `Offset` object by reading from a bitstream.
     *
     * This constructor reads the subkey and the offset value from the given bitstream,
     * initializing the internal state of the `Offset` object.
     *
     * @param bitReader Bitstream reader used to extract the subkey and offset.
     */
    explicit Offset(util::BitReader& bitReader);

    /**
     * @brief Writes the `Offset` object to a bitstream.
     *
     * This method serializes the subkey and the offset value into the specified bitstream,
     * ensuring that the offset information is correctly encoded.
     *
     * @param writer Bitstream writer to serialize the offset data.
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief Retrieves the subkey associated with the offset.
     * @return Constant reference to the subkey string.
     */
    [[nodiscard]] const std::string& getSubkey() const;

    /**
     * @brief Retrieves the numerical offset value.
     * @return 64-bit unsigned integer representing the offset.
     */
    [[nodiscard]] uint64_t getOffset() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_OFFSET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

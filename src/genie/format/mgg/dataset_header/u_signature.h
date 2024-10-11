/**
 * @file
 * @brief Defines the `USignature` class used within the MPEG-G dataset header.
 *
 * The `USignature` class represents a metadata signature for unaligned data blocks.
 * It provides optional fields that can define properties such as a constant length
 * for unaligned blocks. The class offers utilities for reading, writing, and querying
 * the state of its fields, facilitating the handling of unaligned block metadata.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @class USignature
 * @brief Represents a metadata signature for unaligned data blocks.
 *
 * The `USignature` class encapsulates an optional constant length value that provides
 * additional metadata for unaligned blocks in an MPEG-G dataset. This class is primarily
 * used to define specific attributes of unaligned blocks that may be used in data processing
 * and compression. The `USignature` can be present or absent, depending on the dataset configuration.
 */
class USignature {
 private:
    std::optional<uint8_t> const_length;  //!< @brief Optional constant length value for unaligned data blocks.

 public:
    /**
     * @brief Checks for equality between two `USignature` objects.
     *
     * Compares the `const_length` value between this instance and another `USignature` object
     * to determine if they are equal.
     *
     * @param other The other `USignature` instance to compare with.
     * @return `true` if both instances have the same `const_length` value or are both absent, `false` otherwise.
     */
    bool operator==(const USignature& other) const;

    /**
     * @brief Constructs an empty `USignature` object.
     *
     * Initializes the `USignature` with no constant length value, representing the absence of a signature.
     */
    USignature();

    /**
     * @brief Constructs a `USignature` object with a specified constant length value.
     *
     * Initializes the `USignature` with a given constant length value, indicating a specific length
     * for unaligned data blocks.
     *
     * @param _const_length The constant length value for unaligned blocks.
     */
    explicit USignature(uint8_t _const_length);

    /**
     * @brief Constructs a `USignature` object by reading from a bitstream.
     *
     * Reads the constant length value from the given bitstream, if present, initializing
     * the internal state of the `USignature` instance.
     *
     * @param reader Bitstream reader used to extract the constant length value.
     */
    explicit USignature(genie::util::BitReader& reader);

    /**
     * @brief Writes the `USignature` to a bitstream.
     *
     * Serializes the constant length value, if present, into the specified bitstream writer.
     * This method is used when exporting the `USignature` to an MPEG-G compatible format.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Checks if the constant length field is present.
     *
     * Determines if the `const_length` value is set in the `USignature` object.
     *
     * @return `true` if `const_length` is present, `false` otherwise.
     */
    [[nodiscard]] bool isConstLength() const;

    /**
     * @brief Retrieves the constant length value.
     *
     * Returns the constant length value if present. If the value is absent, an exception may be thrown
     * or the behavior may be undefined, depending on the calling context.
     *
     * @return The constant length value.
     */
    [[nodiscard]] uint8_t getConstLength() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `USignature` class used within the MPEG-G dataset parameter set.
 *
 * The `USignature` class represents a metadata signature for unaligned data blocks
 * within a parameter set. It provides an optional field for defining a constant length
 * value, which specifies the length of unaligned blocks in the dataset. This class
 * offers utilities for reading, writing, and modifying the signature, making it easier
 * to handle block metadata.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameterset {

/**
 * @class USignature
 * @brief Represents a U-signature for unaligned blocks in an MPEG-G dataset parameter set.
 *
 * The `USignature` class encapsulates an optional constant length field that provides metadata
 * for unaligned blocks within a parameter set. This class is used to specify properties of
 * unaligned data blocks, such as whether they have a fixed length, and offers methods for
 * reading from and writing to bitstreams.
 */
class USignature {
 private:
    std::optional<uint8_t> u_signature_length;  //!< @brief Optional constant length value for unaligned blocks.

 public:
    /**
     * @brief Checks for equality between two `USignature` objects.
     *
     * Compares the `u_signature_length` value between this instance and another `USignature` object
     * to determine if they are equal.
     *
     * @param other The other `USignature` instance to compare with.
     * @return `true` if both instances have the same `u_signature_length` value or are both absent, `false` otherwise.
     */
    bool operator==(const USignature& other) const;

    /**
     * @brief Constructs an empty `USignature` object.
     *
     * Initializes the `USignature` with no constant length value, representing the absence of a signature.
     */
    USignature();

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
     * @brief Retrieves the presence status of the constant length value.
     *
     * Checks whether the `u_signature_length` field is present, indicating if the signature
     * defines a constant length for unaligned blocks.
     *
     * @return `true` if the constant length field is present, `false` otherwise.
     */
    [[nodiscard]] bool isConstantLength() const;

    /**
     * @brief Retrieves the constant length value if present.
     *
     * Returns the constant length value for unaligned blocks if it is set. If the value is absent,
     * an exception may be thrown or the behavior may be undefined, depending on the context.
     *
     * @return The constant length value.
     */
    [[nodiscard]] uint8_t getConstLength() const;

    /**
     * @brief Sets the constant length value.
     *
     * Configures the `USignature` to include a specific constant length value, which indicates
     * the length of the unaligned blocks in the dataset.
     *
     * @param length The constant length value to set.
     */
    void setConstLength(uint8_t length);

    /**
     * @brief Writes the `USignature` to a bitstream.
     *
     * Serializes the constant length value, if present, into the specified bitstream writer.
     * This method is used when exporting the `USignature` to an MPEG-G compatible format.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameterset

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_U_SIGNATURE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

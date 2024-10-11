/**
 * @file
 * @brief Defines the `UOptions` class used within the MPEG-G dataset header.
 *
 * The `UOptions` class represents a set of optional configurations within the dataset header.
 * It provides fields such as reserved parameters and an optional U-signature, allowing for more
 * flexible handling of unaligned data blocks in MPEG-G datasets. The class offers utilities for
 * reading, writing, and modifying these fields within an MPEG-G file.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/format/mgg/dataset_header/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

/**
 * @class UOptions
 * @brief Represents the U options within an MPEG-G dataset header.
 *
 * The `UOptions` class encapsulates a set of optional fields used in unaligned data blocks.
 * It includes reserved fields for future use, as well as an optional U-signature that provides
 * additional metadata for the unaligned blocks. This class offers methods for reading from and
 * writing to bitstreams, and utilities for checking and modifying its internal state.
 */
class UOptions {
 private:
    uint64_t reserved1;                     //!< @brief Reserved 64-bit value for future extensions or compatibility.
    std::optional<USignature> u_signature;  //!< @brief Optional U-signature containing metadata for unaligned blocks.
    std::optional<uint8_t> reserved2;       //!< @brief Optional reserved 8-bit value for future use.
    bool reserved3;                         //!< @brief Reserved boolean flag.

 public:
    /**
     * @brief Checks for equality between two `UOptions` objects.
     *
     * Compares the reserved fields and the U-signature of this instance with another
     * `UOptions` object to determine if they are equal.
     *
     * @param other The other `UOptions` instance to compare with.
     * @return `true` if all fields match, `false` otherwise.
     */
    bool operator==(const UOptions& other) const;

    /**
     * @brief Constructs a `UOptions` object with the specified reserved fields.
     *
     * Initializes the `UOptions` object with given values for `reserved1` and `reserved3`.
     * Other fields are left as `std::nullopt` by default.
     *
     * @param _reserved1 64-bit reserved field.
     * @param _reserved3 Boolean reserved field.
     */
    explicit UOptions(uint64_t _reserved1 = 0, bool _reserved3 = false);

    /**
     * @brief Constructs a `UOptions` object by reading from a bitstream.
     *
     * Reads and initializes the `UOptions` fields from the provided bitstream.
     * The bitstream is expected to contain the fields in the correct order.
     *
     * @param reader Bitstream reader used to extract the field values.
     */
    explicit UOptions(genie::util::BitReader& reader);

    /**
     * @brief Writes the `UOptions` to a bitstream.
     *
     * Serializes the internal fields of the `UOptions` into the specified bitstream writer.
     * This method is used when exporting the `UOptions` to an MPEG-G compatible format.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Retrieves the value of the `reserved1` field.
     *
     * Returns the 64-bit reserved field value stored in the `UOptions`.
     *
     * @return The value of `reserved1`.
     */
    [[nodiscard]] uint64_t getReserved1() const;

    /**
     * @brief Retrieves the value of the `reserved3` field.
     *
     * Returns the boolean reserved field value stored in the `UOptions`.
     *
     * @return The value of `reserved3`.
     */
    [[nodiscard]] bool getReserved3() const;

    /**
     * @brief Checks if the `reserved2` field is present.
     *
     * Determines if the `reserved2` field is set in the `UOptions` object.
     *
     * @return `true` if `reserved2` is present, `false` otherwise.
     */
    [[nodiscard]] bool hasReserved2() const;

    /**
     * @brief Retrieves the value of the `reserved2` field.
     *
     * Returns the 8-bit reserved field value if present. If the field is not present,
     * an exception may be thrown or the behavior may be undefined.
     *
     * @return The value of `reserved2`.
     */
    [[nodiscard]] uint8_t getReserved2() const;

    /**
     * @brief Checks if the `u_signature` field is present.
     *
     * Determines if the `u_signature` field is set in the `UOptions` object.
     *
     * @return `true` if `u_signature` is present, `false` otherwise.
     */
    [[nodiscard]] bool hasSignature() const;

    /**
     * @brief Retrieves the U-signature if present.
     *
     * Returns a constant reference to the `USignature` object if it is present.
     * If not present, an exception may be thrown or the behavior may be undefined.
     *
     * @return Constant reference to the `USignature` object.
     */
    [[nodiscard]] const USignature& getSignature() const;

    /**
     * @brief Adds a U-signature to the `UOptions`.
     *
     * Sets the U-signature field to the provided `USignature` object, replacing any existing value.
     *
     * @param s The `USignature` object to set.
     */
    void addSignature(USignature s);

    /**
     * @brief Adds a `reserved2` field to the `UOptions`.
     *
     * Sets the `reserved2` field to the provided 8-bit value, replacing any existing value.
     *
     * @param r The 8-bit value to set as `reserved2`.
     */
    void addReserved2(uint8_t r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_HEADER_U_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

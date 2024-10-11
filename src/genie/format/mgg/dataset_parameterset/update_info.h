/**
 * @file
 * @brief Defines the `UpdateInfo` class used within the MPEG-G dataset parameter set.
 *
 * The `UpdateInfo` class represents metadata associated with updates in a dataset parameter set.
 * It encapsulates various flags and optional fields, such as alignment flags, position encoding flags,
 * and a reference to the `USignature` metadata. This class provides methods for reading, writing,
 * and modifying the update information, making it easier to handle MPEG-G parameter set metadata.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/core/constants.h"
#include "genie/format/mgg/dataset_parameterset/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_parameterset {

/**
 * @class UpdateInfo
 * @brief Represents update-related metadata for a dataset parameter set in MPEG-G.
 *
 * The `UpdateInfo` class stores configuration flags and additional metadata that describe
 * specific attributes of updates in a dataset. This includes flags such as the `multiple_alignment_flag`,
 * the `pos_40_bits_flag`, and an optional `USignature` for unaligned block metadata. The class
 * provides utilities for reading and writing the update information to and from bitstreams.
 */
class UpdateInfo {
 private:
    bool multiple_alignment_flag;           //!< @brief Flag indicating if multiple alignment is enabled.
    bool pos_40_bits_flag;                  //!< @brief Flag indicating if positions are encoded with 40 bits.
    core::AlphabetID alphabetId;            //!< @brief Alphabet ID specifying the type of alphabet used in the dataset.
    std::optional<USignature> u_signature;  //!< @brief Optional U-signature for unaligned block metadata.

 public:
    /**
     * @brief Checks for equality between two `UpdateInfo` objects.
     *
     * Compares the alignment flags, position encoding flag, alphabet ID, and the U-signature
     * between this instance and another `UpdateInfo` object to determine if they are equal.
     *
     * @param other The other `UpdateInfo` instance to compare with.
     * @return `true` if both instances have the same flags, alphabet ID, and U-signature, `false` otherwise.
     */
    bool operator==(const UpdateInfo& other) const;

    /**
     * @brief Constructs an `UpdateInfo` object with specified flags and alphabet ID.
     *
     * Initializes the `UpdateInfo` with given values for `multiple_alignment_flag`, `pos_40_bits_flag`,
     * and `alphabetId`. The `u_signature` is left as an empty optional by default.
     *
     * @param _multiple_alignment_flag Boolean indicating if multiple alignment is enabled.
     * @param _pos_40_bits_flag Boolean indicating if positions are encoded with 40 bits.
     * @param _alphabetId The alphabet ID specifying the alphabet used in the dataset.
     */
    UpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag, core::AlphabetID _alphabetId);

    /**
     * @brief Constructs an `UpdateInfo` object by reading from a bitstream.
     *
     * Reads and initializes the alignment flags, position flag, alphabet ID, and U-signature
     * from the specified bitstream.
     *
     * @param reader Bitstream reader used to extract the update information.
     */
    explicit UpdateInfo(genie::util::BitReader& reader);

    /**
     * @brief Writes the `UpdateInfo` to a bitstream.
     *
     * Serializes the alignment flags, position flag, alphabet ID, and the optional U-signature
     * into the specified bitstream writer.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief Adds a U-signature to the `UpdateInfo`.
     *
     * Sets the optional `u_signature` field to the provided `USignature` object, replacing
     * any existing value.
     *
     * @param signature The `USignature` object to set.
     */
    void addUSignature(USignature signature);

    /**
     * @brief Retrieves the value of the `multiple_alignment_flag`.
     *
     * Returns whether multiple alignment is enabled for this update.
     *
     * @return `true` if multiple alignment is enabled, `false` otherwise.
     */
    [[nodiscard]] bool getMultipleAlignmentFlag() const;

    /**
     * @brief Retrieves the value of the `pos_40_bits_flag`.
     *
     * Returns whether positions are encoded using 40 bits in this update.
     *
     * @return `true` if positions are encoded using 40 bits, `false` otherwise.
     */
    [[nodiscard]] bool getPos40BitsFlag() const;

    /**
     * @brief Retrieves the value of the `alphabetId`.
     *
     * Returns the alphabet ID used in this update, indicating the type of alphabet used
     * for encoding sequence data.
     *
     * @return The `core::AlphabetID` for this update.
     */
    [[nodiscard]] core::AlphabetID getAlphabetID() const;

    /**
     * @brief Checks if the `u_signature` field is present.
     *
     * Determines if the `u_signature` field is set in the `UpdateInfo` object.
     *
     * @return `true` if `u_signature` is present, `false` otherwise.
     */
    [[nodiscard]] bool hasUSignature() const;

    /**
     * @brief Retrieves the U-signature if present.
     *
     * Returns a constant reference to the `USignature` object if it is present.
     * If not present, an exception may be thrown or the behavior may be undefined.
     *
     * @return Constant reference to the `USignature` object.
     */
    [[nodiscard]] const USignature& getUSignature() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_parameterset

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @brief Defines the `TransformedSubSeq` class for managing CABAC-transformed subsequences in entropy coding.
 * @details This class handles the properties and configurations for transformed subsequences within a CABAC entropy
 *          coding framework. It provides support for setting up different transforms, binarization, and state variable
 *          configurations, as well as for serializing and deserializing the sequence properties to/from binary and JSON
 *          formats.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <nlohmann/json.hpp>
#include "genie/entropy/paramcabac/binarization.h"
#include "genie/entropy/paramcabac/state_vars.h"
#include "genie/entropy/paramcabac/support_values.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Represents a transformed subsequence used in CABAC entropy coding.
 * @details The `TransformedSubSeq` class manages configurations for subsequences using CABAC (Context-based Adaptive
 *          Binary Arithmetic Coding) transforms. It includes settings for transform ID, support values, binarization
 *          configurations, and state variables. This class provides functionality for serializing, deserializing,
 *          and converting the sequence data to/from JSON.
 */
class TransformedSubSeq {
 public:
    /**
     * @brief Default constructor. Initializes an empty transformed subsequence.
     */
    TransformedSubSeq();

    /**
     * @brief Parameterized constructor for creating a `TransformedSubSeq` with specified configurations.
     * @param _transform_ID_subsym The type of transformation to apply (e.g., equality, match, RLE).
     * @param _support_values The support values for the specified transformation.
     * @param _cabac_binarization The binarization configuration for CABAC coding.
     * @param _subsequence_ID The identifier of the subsequence.
     * @param original Flag indicating whether this is the original subsequence or a derived one.
     * @param _alphabet_ID The identifier for the alphabet used in the subsequence.
     */
    TransformedSubSeq(SupportValues::TransformIdSubsym _transform_ID_subsym, SupportValues&& _support_values,
                      Binarization&& _cabac_binarization, core::GenSubIndex _subsequence_ID, bool original = true,
                      core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);

    /**
     * @brief Deserialization constructor. Initializes the subsequence from a `BitReader`.
     * @param reader The `BitReader` for reading binary data.
     * @param _subsequence_ID The identifier of the subsequence.
     * @param _alphabet_ID The identifier for the alphabet used in the subsequence.
     */
    TransformedSubSeq(util::BitReader& reader, core::GenSubIndex _subsequence_ID,
                      core::AlphabetID _alphabet_ID = core::AlphabetID::ACGTN);

    /**
     * @brief Virtual destructor.
     */
    virtual ~TransformedSubSeq() = default;

    /**
     * @brief Serializes the subsequence to a `BitWriter`.
     * @param writer The `BitWriter` for serializing the data.
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief Retrieves the transformation ID for the subsequence.
     * @return The transformation ID (e.g., equality, match, RLE).
     */
    [[nodiscard]] SupportValues::TransformIdSubsym getTransformIDSubsym() const;

    /**
     * @brief Retrieves the support values for the current transformation.
     * @return The `SupportValues` object.
     */
    [[nodiscard]] const SupportValues& getSupportValues() const;

    /**
     * @brief Retrieves the CABAC binarization configuration.
     * @return The `Binarization` object.
     */
    [[nodiscard]] const Binarization& getBinarization() const;

    /**
     * @brief Retrieves the state variables for the current configuration.
     * @return The `StateVars` object.
     */
    [[nodiscard]] const StateVars& getStateVars() const;

    /**
     * @brief Retrieves the modifiable state variables for the current configuration.
     * @return The `StateVars` object.
     */
    StateVars& getStateVars();

    /**
     * @brief Sets the subsequence identifier.
     * @param _subsequence_ID The new identifier for the subsequence.
     */
    void setSubsequenceID(core::GenSubIndex _subsequence_ID);

    /**
     * @brief Retrieves the alphabet identifier used in the subsequence.
     * @return The `core::AlphabetID` used in the subsequence.
     */
    [[nodiscard]] core::AlphabetID getAlphabetID() const;

    /**
     * @brief Sets the alphabet identifier for the subsequence.
     * @param _alphabet_ID The new alphabet identifier.
     */
    void setAlphabetID(core::AlphabetID _alphabet_ID);

    /**
     * @brief Equality operator for comparing two `TransformedSubSeq` objects.
     * @param val The other `TransformedSubSeq` object to compare with.
     * @return True if both objects are equal, otherwise false.
     */
    bool operator==(const TransformedSubSeq& val) const;

    /**
     * @brief Constructs the subsequence from a JSON object.
     * @param j The JSON object containing the subsequence configuration.
     * @param last_transformed Indicates whether this is the last transformed subsequence.
     */
    explicit TransformedSubSeq(nlohmann::json j, bool last_transformed);

    /**
     * @brief Converts the current subsequence to a JSON object.
     * @return A JSON representation of the subsequence.
     */
    [[nodiscard]] nlohmann::json toJson() const;

 private:
    SupportValues::TransformIdSubsym transform_ID_subsym;  //!< @brief The type of transformation applied.
    SupportValues support_values;     //!< @brief Support values associated with the transformation.
    Binarization cabac_binarization;  //!< @brief Binarization configuration for CABAC coding.
    StateVars state_vars;             //!< @brief State variables for the transformation.

    core::GenSubIndex subsequence_ID = core::GenSub::POS_MAPPING_FIRST;  //!< @brief Identifier for the subsequence.
    core::AlphabetID alphabet_ID = core::AlphabetID::ACGTN;              //!< @brief Identifier for the alphabet used.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

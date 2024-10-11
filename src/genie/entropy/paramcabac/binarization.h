/**
 * @file
 * @brief Defines the `Binarization` class, which represents CABAC binarization configurations in GENIE.
 * @details The `Binarization` class encapsulates the various configurations used for encoding symbols into binary
 *          sequences using different CABAC (Context-based Adaptive Binary Arithmetic Coding) binarization schemes.
 *          It includes parameters for the binarization type, context models, and options for bypassing regular CABAC
 *          coding modes. Additionally, the class supports reading from and writing to bitstreams, and converting
 *          the configuration to and from JSON format for serialization.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/paramcabac/binarization_parameters.h"
#include "genie/entropy/paramcabac/context.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Represents a CABAC binarization configuration used in GENIE.
 * @details The `Binarization` class defines the structure and behavior for encoding data using CABAC binarization.
 *          It holds parameters for different binarization types (e.g., TU, EG, SEG) and context models for efficient
 *          encoding and decoding.
 */
class Binarization {
 public:
    /**
     * @brief Default constructor for `Binarization`.
     */
    Binarization();

    /**
     * @brief Constructs a `Binarization` object with specified parameters.
     * @param _binarization_ID Identifier for the type of binarization used (e.g., TU, EG, SEG).
     * @param _bypass_flag Flag indicating whether to bypass regular CABAC coding (true = bypass).
     * @param _cabac_binarization_parameters The parameters for the chosen binarization type.
     * @param _cabac_context_parameters The context models used for CABAC encoding.
     */
    Binarization(BinarizationParameters::BinarizationId _binarization_ID, bool _bypass_flag,
                 BinarizationParameters&& _cabac_binarization_parameters, Context&& _cabac_context_parameters);

    /**
     * @brief Constructs a `Binarization` object by reading from a bitstream.
     * @param output_symbol_size The size of the output symbols.
     * @param coding_subsym_size The size of the coding subsymbols.
     * @param reader Reference to a `BitReader` for reading from the bitstream.
     */
    Binarization(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Binarization() = default;

    /**
     * @brief Sets the context parameters for CABAC encoding.
     * @param _cabac_context_parameters The context model parameters.
     */
    void setContextParameters(Context&& _cabac_context_parameters);

    /**
     * @brief Serializes the `Binarization` object to a bitstream.
     * @param writer Reference to a `BitWriter` for writing to the bitstream.
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief Retrieves the binarization type identifier.
     * @return The `BinarizationId` type of the current binarization configuration.
     */
    [[nodiscard]] BinarizationParameters::BinarizationId getBinarizationID() const;

    /**
     * @brief Checks whether the bypass flag is set.
     * @return True if bypass flag is set, false otherwise.
     */
    [[nodiscard]] bool getBypassFlag() const;

    /**
     * @brief Retrieves the binarization parameters.
     * @return A constant reference to the `BinarizationParameters` object.
     */
    [[nodiscard]] const BinarizationParameters& getCabacBinarizationParameters() const;

    /**
     * @brief Retrieves the context parameters.
     * @return A constant reference to the `Context` object.
     */
    [[nodiscard]] const Context& getCabacContextParameters() const;

    /**
     * @brief Retrieves the number of binarization parameters used in the current configuration.
     * @return The number of binarization parameters.
     */
    uint8_t getNumBinarizationParams();

    /**
     * @brief Equality comparison operator for `Binarization` objects.
     * @param bin Reference to another `Binarization` object.
     * @return True if both objects are equal, false otherwise.
     */
    bool operator==(const Binarization& bin) const;

    /**
     * @brief Constructs a `Binarization` object from a JSON representation.
     * @param j JSON object containing the binarization configuration.
     */
    explicit Binarization(nlohmann::json j);

    /**
     * @brief Converts the `Binarization` object to a JSON representation.
     * @return A JSON object representing the binarization configuration.
     */
    [[nodiscard]] nlohmann::json toJson() const;

 private:
    BinarizationParameters::BinarizationId binarization_ID;  //!< Identifier for the type of binarization used.
    uint8_t bypass_flag;                                   //!< Flag indicating if CABAC coding is bypassed (1 = true).
    BinarizationParameters cabac_binarization_parameters;  //!< Parameters for the chosen binarization type.
    Context cabac_context_parameters;                      //!< Context model parameters for CABAC encoding.
};

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

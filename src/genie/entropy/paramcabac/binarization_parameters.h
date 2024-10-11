/**
 * @file
 * @brief Defines the `BinarizationParameters` class for handling CABAC binarization parameters in GENIE.
 * @details The `BinarizationParameters` class encapsulates the parameters required for different binarization schemes
 *          used in CABAC (Context-based Adaptive Binary Arithmetic Coding) entropy coding. This includes configurations
 *          for multiple binarization types, such as BI, TU, EG, SEG, TEG, and more. The class supports reading from and
 *          writing to bitstreams, as well as conversion to and from JSON format for parameter serialization.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Defines parameters for CABAC binarization schemes.
 * @details This class encapsulates different configurations for CABAC binarization schemes, which are used to encode
 *          symbols into binary sequences for entropy coding. It supports multiple binarization types, such as binary
 *          (BI), truncated unary (TU), exponential golomb (EG), and several variations thereof.
 */
class BinarizationParameters {
 public:
    /**
     * @brief Enumeration for different binarization types.
     */
    enum class BinarizationId : uint8_t {
        BI = 0,     //!< Binary encoding.
        TU = 1,     //!< Truncated Unary encoding.
        EG = 2,     //!< Exponential Golomb encoding.
        SEG = 3,    //!< Signed Exponential Golomb encoding.
        TEG = 4,    //!< Truncated Exponential Golomb encoding.
        STEG = 5,   //!< Signed Truncated Exponential Golomb encoding.
        SUTU = 6,   //!< Subsymbol Unary-Truncated Unary encoding.
        SSUTU = 7,  //!< Signed Subsymbol Unary-Truncated Unary encoding.
        DTU = 8,    //!< Delta Truncated Unary encoding.
        SDTU = 9    //!< Signed Delta Truncated Unary encoding.
    };

    /**
     * @brief Default constructor for `BinarizationParameters`.
     */
    BinarizationParameters();

    /**
     * @brief Constructs a `BinarizationParameters` object from a bitstream.
     * @param binID The type of binarization used.
     * @param reader Reference to a `BitReader` for reading from the bitstream.
     */
    BinarizationParameters(BinarizationId binID, util::BitReader &reader);

    /**
     * @brief Constructs a `BinarizationParameters` object with specified parameters.
     * @param _binarization_id The binarization type identifier.
     * @param params Vector of binarization parameters.
     */
    BinarizationParameters(const BinarizationId &_binarization_id, std::vector<uint8_t> params);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~BinarizationParameters() = default;

    /**
     * @brief Serializes the binarization parameters to a bitstream.
     * @param binID The binarization type identifier.
     * @param writer Reference to a `BitWriter` for writing to the bitstream.
     */
    virtual void write(BinarizationId binID, util::BitWriter &writer) const;

    /**
     * @brief Retrieves the maximum coding value for standard binarization.
     * @return The maximum coding value for standard binarization.
     */
    [[nodiscard]] uint8_t getCMax() const;

    /**
     * @brief Retrieves the maximum coding value for Truncated Exponential Golomb binarization.
     * @return The maximum coding value for TEG binarization.
     */
    [[nodiscard]] uint8_t getCMaxTeg() const;

    /**
     * @brief Retrieves the maximum coding value for Delta Truncated Unary binarization.
     * @return The maximum coding value for DTU binarization.
     */
    [[nodiscard]] uint8_t getCMaxDtu() const;

    /**
     * @brief Retrieves the split unit size parameter for certain binarization schemes.
     * @return The split unit size parameter.
     */
    [[nodiscard]] uint8_t getSplitUnitSize() const;

    /**
     * @brief Determines the number of binarization parameters required for a given binarization type.
     * @param binarizationId The binarization type identifier.
     * @return The number of parameters required for the specified binarization type.
     */
    static uint8_t getNumBinarizationParams(BinarizationParameters::BinarizationId binarizationId);

    /**
     * @brief Equality comparison operator for `BinarizationParameters` objects.
     * @param bin Reference to another `BinarizationParameters` object.
     * @return True if both objects are equal, false otherwise.
     */
    bool operator==(const BinarizationParameters &bin) const;

    /**
     * @brief Constructs a `BinarizationParameters` object from a JSON representation.
     * @param j JSON object containing the binarization parameters.
     * @param binID The binarization type identifier.
     */
    BinarizationParameters(nlohmann::json j, BinarizationId binID);

    /**
     * @brief Converts the `BinarizationParameters` object to a JSON representation.
     * @param binID The binarization type identifier.
     * @return A JSON object representing the binarization parameters.
     */
    [[nodiscard]] nlohmann::json toJson(BinarizationId binID) const;

 private:
    uint8_t cmax{0};             //!< @brief Maximum coding value for standard binarization.
    uint8_t cmax_teg{0};         //!< @brief Maximum coding value for TEG binarization.
    uint8_t cmax_dtu{0};         //!< @brief Maximum coding value for DTU binarization.
    uint8_t split_unit_size{0};  //!< @brief Split unit size for certain binarization schemes.

    // Array defining the number of parameters for each binarization type.
    static uint8_t numParams[unsigned(BinarizationId::SDTU) + 1u];  //!< @brief Number of parameters for each type.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file binarization_parameters.h
 * @brief Defines the `BinarizationParameters` class for handling CABAC
 * binarization parameters in Genie
 * @details The `BinarizationParameters` class encapsulates the parameters
 * required for different binarization schemes used in CABAC (Context-based
 * Adaptive Binary Arithmetic Coding) entropy coding. This includes
 * configurations for multiple binarization types, such as BI, TU, EG, SEG, TEG,
 * and more. The class supports reading from and writing to bitstreams, as well
 * as conversion to and from JSON format for parameter serialization.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Defines parameters for CABAC binarization schemes.
 * @details This class encapsulates different configurations for CABAC
 * binarization schemes, which are used to encode symbols into binary sequences
 * for entropy coding. It supports multiple binarization types, such as binary
 *          (BI), truncated unary (TU), exponential golomb (EG), and several
 * variations thereof.
 */
class BinarizationParameters final {
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
   * @param bin_id The type of binarization used.
   * @param reader Reference to a `BitReader` for reading from the bitstream.
   */
  BinarizationParameters(BinarizationId bin_id, util::BitReader& reader);

  /**
   * @brief Constructs a `BinarizationParameters` object with specified
   * parameters.
   * @param binarization_id The binarization type identifier.
   * @param params Vector of binarization parameters.
   */
  BinarizationParameters(const BinarizationId& binarization_id,
                         const std::vector<uint8_t>& params);

  /**
   * @brief Serializes the binarization parameters to a bitstream.
   * @param bin_id The binarization type identifier.
   * @param writer Reference to a `BitWriter` for writing to the bitstream.
   */
  void write(BinarizationId bin_id, util::BitWriter& writer) const;

  /**
   * @brief Retrieves the maximum coding value for standard binarization.
   * @return The maximum coding value for standard binarization.
   */
  [[nodiscard]] uint8_t GetCMax() const;

  /**
   * @brief Retrieves the maximum coding value for Truncated Exponential Golomb
   * binarization.
   * @return The maximum coding value for TEG binarization.
   */
  [[nodiscard]] uint8_t GetCMaxTeg() const;

  /**
   * @brief Retrieves the maximum coding value for Delta Truncated Unary
   * binarization.
   * @return The maximum coding value for DTU binarization.
   */
  [[nodiscard]] uint8_t GetCMaxDtu() const;

  /**
   * @brief Retrieves the split unit Size parameter for certain binarization
   * schemes.
   * @return The split unit Size parameter.
   */
  [[nodiscard]] uint8_t GetSplitUnitSize() const;

  /**
   * @brief Determines the number of binarization parameters required for a
   * given binarization type.
   * @param binarization_id The binarization type identifier.
   * @return The number of parameters required for the specified binarization
   * type.
   */
  static uint8_t GetNumBinarizationParams(BinarizationId binarization_id);

  /**
   * @brief Equality comparison operator for `BinarizationParameters` objects.
   * @param bin Reference to another `BinarizationParameters` object.
   * @return True if both objects are equal, false otherwise.
   */
  bool operator==(const BinarizationParameters& bin) const;

  /**
   * @brief Constructs a `BinarizationParameters` object from a JSON
   * representation.
   * @param j JSON object containing the binarization parameters.
   * @param bin_id The binarization type identifier.
   */
  BinarizationParameters(nlohmann::json j, BinarizationId bin_id);

  /**
   * @brief Converts the `BinarizationParameters` object to a JSON
   * representation.
   * @param bin_id The binarization type identifier.
   * @return A JSON object representing the binarization parameters.
   */
  [[nodiscard]] nlohmann::json ToJson(BinarizationId bin_id) const;

 private:
  uint8_t cmax_{0};  //!< @brief Maximum coding value for standard binarization.
  uint8_t cmax_teg_{0};  //!< @brief Maximum coding value for TEG binarization.
  uint8_t cmax_dtu_{0};  //!< @brief Maximum coding value for DTU binarization.
  uint8_t split_unit_size_{
      0};  //!< @brief Split unit Size for certain binarization schemes.

  // Array defining the number of parameters for each binarization type.
  static uint8_t
      num_params_[static_cast<unsigned>(BinarizationId::SDTU) +
                  1u];  //!< @brief Number of parameters for each type.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_BINARIZATION_PARAMETERS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
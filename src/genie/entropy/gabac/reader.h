/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the Reader class for decoding streams in GENIE using
 * Binary Arithmetic Coding (BAC).
 *
 * This file contains the declaration of the Reader class, which provides
 * methods to Decode streams using various binarization schemes, such as BI, TU,
 * EG, TEG, SUTU, and DTU. The class also supports CABAC (Context-Adaptive
 * Binary Arithmetic Coding) and bypass decoding modes.
 *
 * @details The Reader class is used to interpret encoded bitstreams by reading
 * symbols according to specified binarization parameters. It supports both
 * CABAC and bypass decoding schemes, enabling efficient entropy decoding for a
 * variety of compression schemes.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_READER_H_
#define SRC_GENIE_ENTROPY_GABAC_READER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>

#include "genie/entropy/gabac/binary_arithmetic_decoder.h"
#include "genie/entropy/gabac/bit_input_stream.h"

// -----------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Decodes bitstreams using Binary Arithmetic Coding (BAC) and
 * context-based models.
 */
class Reader {
 public:
  /**
   * @brief Constructs a Reader object for decoding a given bitstream.
   *
   * Initializes a Reader with the specified DataBlock and configures the
   * bypass and context models based on the provided flags.
   *
   * @param bitstream Pointer to the DataBlock containing the bitstream.
   * @param bypass_flag Boolean flag to determine if bypass mode is enabled.
   * @param num_contexts Number of context models to use for CABAC.
   */
  explicit Reader(util::DataBlock* bitstream, bool bypass_flag = true,
                  uint64_t num_contexts = 0);

  /**
   * @brief Destructor to clean up the Reader object.
   */
  ~Reader();

  /**
   * @brief Reads a symbol using Binary Integer (BI) bypass decoding.
   * @param bin_params Parameters controlling the BI decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsBIbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Binary Integer (BI) CABAC decoding.
   * @param bin_params Parameters controlling the BI decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsBIcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Truncated Unary (TU) bypass decoding.
   * @param bin_params Parameters controlling the TU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsTUbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Truncated Unary (TU) CABAC decoding.
   * @param bin_params Parameters controlling the TU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsTUcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Exponential Golomb (EG) bypass decoding.
   * @param bin_params Parameters controlling the EG decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsEGbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Exponential Golomb (EG) CABAC decoding.
   * @param bin_params Parameters controlling the EG decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsEGcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Truncated Exponential Golomb (TEG) bypass
   * decoding.
   * @param bin_params Parameters controlling the TEG decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsTeGbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Truncated Exponential Golomb (TEG) CABAC
   * decoding.
   * @param bin_params Parameters controlling the TEG decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsTeGcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Split Unary Truncated Unary (SUTU) bypass
   * decoding.
   * @param bin_params Parameters controlling the SUTU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsSutUbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Split Unary Truncated Unary (SUTU) CABAC
   * decoding.
   * @param bin_params Parameters controlling the SUTU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsSutUcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Double Truncated Unary (DTU) bypass decoding.
   * @param bin_params Parameters controlling the DTU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsDtUbypass(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using Double Truncated Unary (DTU) CABAC decoding.
   * @param bin_params Parameters controlling the DTU decoding.
   * @return The decoded symbol.
   */
  uint64_t ReadAsDtUcabac(const std::vector<unsigned int>& bin_params);

  /**
   * @brief Reads a symbol using a Look-Up Table (LUT) for symbol decoding.
   * @param coding_subsym_size Size of the coding sub-symbol in bits.
   * @return The decoded symbol from the LUT.
   */
  uint64_t ReadLutSymbol(uint8_t coding_subsym_size);

  /**
   * @brief Reads a sign flag from the bitstream.
   * @return The sign flag (true for negative, false for positive).
   */
  bool ReadSignFlag();

  /**
   * @brief Initializes the Reader for a new bitstream.
   */
  void Start();

  /**
   * @brief Closes the Reader and returns the number of bits read.
   * @return Number of bits read from the bitstream.
   */
  size_t Close();

  /**
   * @brief Resets the Reader's state, clearing context models and stream.
   */
  void Reset();

 private:
  /// Input bitstream for reading.
  BitInputStream m_bit_input_stream_;

  /// CABAC decoder for arithmetic coding.
  BinaryArithmeticDecoder m_dec_bin_cabac_;

  /// Indicates if bypass mode is enabled.
  bool m_bypass_flag_;

  /// Number of contexts available for CABAC decoding.
  uint64_t m_num_contexts_;

  /// Context models for CABAC decoding.
  std::vector<ContextModel> m_context_models_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_READER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file writer.h
 * @brief Header file for the `Writer` class, which handles CABAC/GABAC-based
 * encoding of symbols.
 * @details This class encapsulates the encoding mechanisms for various
 * binarization schemes (e.g., BI, TU, EG) using both CABAC and bypass modes. It
 * provides functionalities for symbol encoding based on contexts and context
 *          models. The class uses `BinaryArithmeticEncoder` for CABAC encoding
 * and supports multiple context management.
 * @copyright This file is part of Genie  See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_WRITER_H_
#define SRC_GENIE_ENTROPY_GABAC_WRITER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/entropy/gabac/binary_arithmetic_encoder.h"
#include "genie/entropy/gabac/streams.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {
/**
 * @brief Handles the encoding of symbols using different binarization schemes.
 * @details The `Writer` class provides methods for encoding symbols using
 * bypass and CABAC modes for various binarization techniques (BI, TU, EG,
 * etc.). It manages a bitstream writer and a binary arithmetic encoder, and
 * allows for context-based symbol encoding.
 */
class Writer {
 public:
  /**
   * @brief Constructs a `Writer` object with specified configurations.
   * @param bitstream Pointer to the output bitstream.
   * @param bypass_flag Flag indicating whether bypass mode is enabled (true =
   * bypass mode).
   * @param num_contexts The number of contexts for CABAC encoding (default =
   * 0).
   */
  explicit Writer(OBufferStream *bitstream, bool bypass_flag = true,
                  uint64_t num_contexts = 0);

  /**
   * @brief Destructor for `Writer`.
   */
  ~Writer();

  /**
   * @brief Initializes the writer for symbol encoding.
   * @details Sets up the internal bitstream and context models, preparing the
   * writer for encoding operations.
   */
  void Start();

  /**
   * @brief Finalizes the writer and closes the bitstream.
   * @details Completes the encoding process, flushes the remaining bits, and
   * closes the output stream.
   */
  void Close();

  /**
   * @brief Resets the internal state of the writer.
   * @details Clears any context models and resets the binary arithmetic
   * encoder for a fresh encoding session.
   */
  void Reset();

  // -------------------------------------------------------------------------
  // Encoding Methods for Binarization Schemes (Bypass and CABAC Modes)
  // -------------------------------------------------------------------------

  /**
   * @brief Encodes a symbol using the BI (Binary) binarization in bypass
   * mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the BI binarization scheme.
   */
  void WriteAsBiBypass(uint64_t input,
                       const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the BI (Binary) binarization in CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the BI binarization scheme.
   */
  void WriteAsBiCabac(uint64_t input,
                      const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the TU (Truncated Unary) binarization in
   * bypass mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the TU binarization scheme.
   */
  void WriteAsTuBypass(uint64_t input,
                       const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the TU (Truncated Unary) binarization in
   * CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the TU binarization scheme.
   */
  void WriteAsTuCabac(uint64_t input,
                      const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the EG (Exponential Golomb) binarization in
   * bypass mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the EG binarization scheme.
   */
  void WriteAsEgBypass(uint64_t input,
                       const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the EG (Exponential Golomb) binarization in
   * CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the EG binarization scheme.
   */
  void WriteAsEgCabac(uint64_t input,
                      const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the TEG (Truncated Exponential Golomb)
   * binarization in bypass mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the TEG binarization scheme.
   */
  void WriteAsTegBypass(uint64_t input,
                        const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the TEG (Truncated Exponential Golomb)
   * binarization in CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the TEG binarization scheme.
   */
  void WriteAsTegCabac(uint64_t input,
                       const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the SUTU (Sub symbol Unary-Truncated Unary)
   * binarization in bypass mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the SUTU binarization scheme.
   */
  void WriteAsSutuBypass(uint64_t input,
                         const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the SUTU (Sub symbol Unary-Truncated Unary)
   * binarization in CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the SUTU binarization scheme.
   */
  void WriteAsSutuCabac(uint64_t input,
                        const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the DTU (Delta Truncated Unary)
   * binarization in bypass mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the DTU binarization scheme.
   */
  void WriteAsDtuBypass(uint64_t input,
                        const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using the DTU (Delta Truncated Unary)
   * binarization in CABAC mode.
   * @param input The input symbol to encode.
   * @param bin_params Parameters for the DTU binarization scheme.
   */
  void WriteAsDtuCabac(uint64_t input,
                       const std::vector<unsigned int> &bin_params);

  /**
   * @brief Encodes a symbol using LUT (Look-Up Table) encoding.
   * @param input The input symbol to encode.
   * @param coding_sub_sym_size The Size of the coding sub symbol.
   */
  void WriteLutSymbol(uint64_t input, uint8_t coding_sub_sym_size);

  /**
   * @brief Encodes a sign flag for a given signed input.
   * @param input The signed input value for which the sign flag is to be
   * encoded.
   */
  [[maybe_unused]] void WriteSignFlag(int64_t input);

 private:
  util::BitWriter bit_output_stream_;
  BinaryArithmeticEncoder binary_arithmetic_encoder_;
  bool bypass_flag_;
  uint64_t num_contexts_;
  std::vector<ContextModel> context_models_;
};

// -----------------------------------------------------------------------------
}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_WRITER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the `TransformedSymbolsDecoder` class for decoding
 * transformed symbols.
 *
 * This file defines the `TransformedSymbolsDecoder` class, which is responsible
 * for decoding symbols that have undergone transformations like binarization,
 * context modeling, and LUT-based reordering. The class supports decoding for
 * different coding orders and provides mechanisms for handling various
 * CABAC-related operations, such as context selection and symbol decoding.
 *
 * @details The `TransformedSymbolsDecoder` class offers various methods to
 * handle symbol decoding in different transformation orders (Order-0, Order-1,
 * and Order-2). It utilizes bitstream readers, context selectors, and LUT-based
 * transformations to revert the transformed symbols back to their original
 * form. This class is designed to operate with MPEG-G CABAC configurations.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/entropy/gabac/context_selector.h"
#include "genie/entropy/gabac/decode_transformed_sub_seq.h"
#include "genie/entropy/gabac/luts_sub_symbol_transform.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Class for decoding symbols that have undergone various
 * transformations.
 *
 * The `TransformedSymbolsDecoder` class provides the capability to Decode
 * symbols that were transformed using techniques like binarization, context
 * modeling, and LUT-based sub symbol reordering. It supports various orders of
 * transformation, ranging from Order-0 (no dependency) to Order-2 (inter-symbol
 * dependencies). The class maintains internal state for the number of symbols
 * encoded and decoded, as well as configurations for decoding parameters.
 */
class TransformedSymbolsDecoder {
 public:
  /**
   * @brief Constructs a `TransformedSymbolsDecoder` object from a given
   * bitstream and configuration.
   *
   * This constructor initializes the decoder using a reference to the input
   * `bitstream` and the `transformed_sub_seq_conf` configuration, which
   * specifies how the subsequence was encoded. The number of symbols to be
   * decoded is set using `num_encoded_symbols_`.
   *
   * @param bitstream Pointer to the input bitstream containing encoded data.
   * @param transformed_sub_seq_conf Configuration for the transformed
   * subsequence.
   * @param num_symbols The total number of symbols encoded in the input
   * stream.
   */
  explicit TransformedSymbolsDecoder(
      util::DataBlock* bitstream,
      const paramcabac::TransformedSubSeq& transformed_sub_seq_conf,
      unsigned int num_symbols);

  /**
   * @brief Copy constructor for `TransformedSymbolsDecoder`.
   *
   * Creates a copy of the specified `TransformedSymbolsDecoder` object.
   *
   * @param transformed_sub_seq_decoder The `TransformedSymbolsDecoder` object
   * to copy.
   */
  TransformedSymbolsDecoder(
      const TransformedSymbolsDecoder& transformed_sub_seq_decoder);

  /**
   * @brief Default destructor.
   *
   * Releases any allocated resources and performs cleanup operations.
   */
  ~TransformedSymbolsDecoder() = default;

  /**
   * @brief Decodes the next symbol from the bitstream.
   *
   * This function decodes the next symbol from the input bitstream, using the
   * current configuration and context models. If the transformation order is
   * greater than 0, the function uses the provided `dep_symbol` as a
   * dependency for context modeling.
   *
   * @param dep_symbol Pointer to the dependency symbol (if any) for
   * context-based decoding.
   * @return The value of the decoded symbol.
   */
  uint64_t DecodeNextSymbol(const uint64_t* dep_symbol);

  /**
   * @brief Retrieves the number of symbols remaining for decoding.
   *
   * This function returns the number of symbols that are yet to be decoded
   * from the input bitstream.
   *
   * @return The number of symbols available for decoding.
   */
  [[nodiscard]] size_t SymbolsAvail() const;

 private:
  /**
   * @brief Decodes the next symbol using Order-0 transformation (no
   * dependency).
   *
   * This function handles decoding for Order-0 transformations, where symbols
   * are independent of each other. It reads the symbols directly from the
   * bitstream.
   *
   * @return The value of the decoded symbol.
   */
  uint64_t DecodeNextSymbolOrder0();

  /**
   * @brief Decodes the next symbol using Order-1 transformation (dependency
   * on previous symbol).
   *
   * This function handles decoding for Order-1 transformations, where each
   * symbol depends on the previous symbol (dep_symbol). It utilizes the LUT
   * and context models for decoding.
   *
   * @param dep_symbol Pointer to the dependency symbol for context modeling.
   * @return The value of the decoded symbol.
   */
  uint64_t DecodeNextSymbolOrder1(const uint64_t* dep_symbol);

  /**
   * @brief Decodes the next symbol using Order-2 transformation (inter-symbol
   * dependencies).
   *
   * This function handles decoding for Order-2 transformations, where symbols
   * depend on both previous symbols and additional parameters.
   *
   * @return The value of the decoded symbol.
   */
  uint64_t DecodeNextSymbolOrder2();

  // Member variables for maintaining decoding state and configurations.
  unsigned int num_encoded_symbols_;  //!< @brief Total number of symbols to
                                      //!< be decoded.
  unsigned int
      num_decoded_symbols_;  //!< @brief Number of symbols decoded so far.

  uint8_t output_symbol_size_;  //!< @brief Size of the output symbol in bits.
  uint8_t
      coding_sub_sym_size_;   //!< @brief Size of the coding sub symbol in bits.
  uint8_t coding_order_;      //!< @brief Coding order (0, 1, or 2).
  uint64_t sub_symbol_mask_;  //!< @brief Mask used for extracting sub symbols.
  uint8_t num_sub_symbols_;   //!< @brief Number of sub symbols per symbol.
  uint8_t num_luts_;          //!< @brief Number of LUTs used.
  uint8_t num_previous_bits_;  //!< @brief Number of previous symbols used for
                               //!< dependencies.

  Reader reader_;  //!< @brief Bitstream reader for CABAC decoding.
  ContextSelector
      ctx_selector_;  //!< @brief Context selector for CABAC decoding.
  LuTsSubSymbolTransform
      inv_luts_sub_symbol_transform_;  //!< @brief LUT-based sub symbol
                                       //!< transformation handler.

  bool diff_enabled_;     //!< @brief Flag to indicate if differential coding is
                          //!< enabled.
  bool custom_c_max_tu_;  //!< @brief Flag to indicate if custom CMax is used
                          //!< for TU binarization.
  uint64_t
      default_c_max_;  //!< @brief Default CMax value used for CABAC decoding.

  paramcabac::BinarizationParameters::BinarizationId
      bin_id_;  //!< @brief Binarization method used for decoding.
  std::vector<unsigned int> bin_params_;  //!< @brief Parameters for the
                                          //!< specified binarization method.
  BinFunc binarization_;  //!< @brief Binarization function pointer.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_TRANSFORMED_SYMBOLS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

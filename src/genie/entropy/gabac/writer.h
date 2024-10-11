/**
 * @file
 * @brief Header file for the `Writer` class, which handles CABAC/GABAC-based encoding of symbols.
 * @details This class encapsulates the encoding mechanisms for various binarization schemes (e.g., BI, TU, EG) using
 *          both CABAC and bypass modes. It provides functionalities for symbol encoding based on contexts and context
 *          models. The class uses `BinaryArithmeticEncoder` for CABAC encoding and supports multiple context
 * management.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_WRITER_H_
#define SRC_GENIE_ENTROPY_GABAC_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/binary-arithmetic-encoder.h"
#include "genie/entropy/gabac/streams.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;  //!< Forward declaration for `DataBlock`.
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Handles the encoding of symbols using different binarization schemes.
 * @details The `Writer` class provides methods for encoding symbols using bypass and CABAC modes for various
 *          binarization techniques (BI, TU, EG, etc.). It manages a bitstream writer and a binary arithmetic
 *          encoder, and allows for context-based symbol encoding.
 */
class Writer {
 public:
    /**
     * @brief Constructs a `Writer` object with specified configurations.
     * @param constbitstream Pointer to the output bitstream.
     * @param bypassFlag Flag indicating whether bypass mode is enabled (true = bypass mode).
     * @param numContexts The number of contexts for CABAC encoding (default = 0).
     */
    explicit Writer(OBufferStream* constbitstream, bool bypassFlag = true, uint64_t numContexts = 0);

    /**
     * @brief Destructor for `Writer`.
     */
    ~Writer();

    /**
     * @brief Initializes the writer for symbol encoding.
     * @details Sets up the internal bitstream and context models, preparing the writer for encoding operations.
     */
    void start();

    /**
     * @brief Finalizes the writer and closes the bitstream.
     * @details Completes the encoding process, flushes the remaining bits, and closes the output stream.
     */
    void close();

    /**
     * @brief Resets the internal state of the writer.
     * @details Clears any context models and resets the binary arithmetic encoder for a fresh encoding session.
     */
    void reset();

    // -----------------------------------------------------------------------------------------------------------------
    // Encoding Methods for Binarization Schemes (Bypass and CABAC Modes)
    // -----------------------------------------------------------------------------------------------------------------

    /**
     * @brief Encodes a symbol using the BI (Binary) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the BI binarization scheme.
     */
    void writeAsBIbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the BI (Binary) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the BI binarization scheme.
     */
    void writeAsBIcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the TU (Truncated Unary) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the TU binarization scheme.
     */
    void writeAsTUbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the TU (Truncated Unary) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the TU binarization scheme.
     */
    void writeAsTUcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the EG (Exponential Golomb) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the EG binarization scheme.
     */
    void writeAsEGbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the EG (Exponential Golomb) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the EG binarization scheme.
     */
    void writeAsEGcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the TEG (Truncated Exponential Golomb) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the TEG binarization scheme.
     */
    void writeAsTEGbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the TEG (Truncated Exponential Golomb) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the TEG binarization scheme.
     */
    void writeAsTEGcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the SUTU (Subsymbol Unary-Truncated Unary) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the SUTU binarization scheme.
     */
    void writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the SUTU (Subsymbol Unary-Truncated Unary) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the SUTU binarization scheme.
     */
    void writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the DTU (Delta Truncated Unary) binarization in bypass mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the DTU binarization scheme.
     */
    void writeAsDTUbypass(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using the DTU (Delta Truncated Unary) binarization in CABAC mode.
     * @param input The input symbol to encode.
     * @param binParams Parameters for the DTU binarization scheme.
     */
    void writeAsDTUcabac(uint64_t input, const std::vector<unsigned int>& binParams);

    /**
     * @brief Encodes a symbol using LUT (Look-Up Table) encoding.
     * @param input The input symbol to encode.
     * @param codingSubsymSize The size of the coding subsymbol.
     */
    void writeLutSymbol(uint64_t input, uint8_t codingSubsymSize);

    /**
     * @brief Encodes a sign flag for a given signed input.
     * @param input The signed input value for which the sign flag is to be encoded.
     */
    void writeSignFlag(int64_t input);

 private:
    util::BitWriter m_bitOutputStream;  //!< The `BitWriter` object for writing encoded symbols to the output stream.

    BinaryArithmeticEncoder m_binaryArithmeticEncoder;  //!< The binary arithmetic encoder for CABAC-based encoding.

    bool m_bypassFlag;       //!< Flag indicating if the encoder is in bypass mode (true = bypass).
    uint64_t m_numContexts;  //!< Number of contexts for CABAC encoding.

    std::vector<ContextModel> m_contextModels;  //!< The context models used for CABAC encoding.
};

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

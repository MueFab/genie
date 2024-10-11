/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the Reader class for decoding streams in GENIE using Binary Arithmetic Coding (BAC).
 *
 * This file contains the declaration of the Reader class, which provides methods to decode
 * streams using various binarization schemes, such as BI, TU, EG, TEG, SUTU, and DTU.
 * The class also supports CABAC (Context-Adaptive Binary Arithmetic Coding) and bypass
 * decoding modes.
 *
 * @details The Reader class is used to interpret encoded bitstreams by reading symbols
 * according to specified binarization parameters. It supports both CABAC and bypass
 * decoding schemes, enabling efficient entropy decoding for a variety of compression schemes.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_READER_H_
#define SRC_GENIE_ENTROPY_GABAC_READER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>
#include "genie/entropy/gabac/binary-arithmetic-decoder.h"
#include "genie/entropy/gabac/bit-input-stream.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {
class DataBlock;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Decodes bitstreams using Binary Arithmetic Coding (BAC) and context-based models.
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
     * @param bypassFlag Boolean flag to determine if bypass mode is enabled.
     * @param numContexts Number of context models to use for CABAC.
     */
    explicit Reader(util::DataBlock* bitstream, bool bypassFlag = true, uint64_t numContexts = 0);

    /**
     * @brief Destructor to clean up the Reader object.
     */
    ~Reader();

    /**
     * @brief Reads a symbol using Binary Integer (BI) bypass decoding.
     * @param binParams Parameters controlling the BI decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsBIbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Binary Integer (BI) CABAC decoding.
     * @param binParams Parameters controlling the BI decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsBIcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Truncated Unary (TU) bypass decoding.
     * @param binParams Parameters controlling the TU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsTUbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Truncated Unary (TU) CABAC decoding.
     * @param binParams Parameters controlling the TU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsTUcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Exponential Golomb (EG) bypass decoding.
     * @param binParams Parameters controlling the EG decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsEGbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Exponential Golomb (EG) CABAC decoding.
     * @param binParams Parameters controlling the EG decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsEGcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Truncated Exponential Golomb (TEG) bypass decoding.
     * @param binParams Parameters controlling the TEG decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsTEGbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Truncated Exponential Golomb (TEG) CABAC decoding.
     * @param binParams Parameters controlling the TEG decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsTEGcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Split Unary Truncated Unary (SUTU) bypass decoding.
     * @param binParams Parameters controlling the SUTU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsSUTUbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Split Unary Truncated Unary (SUTU) CABAC decoding.
     * @param binParams Parameters controlling the SUTU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsSUTUcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Double Truncated Unary (DTU) bypass decoding.
     * @param binParams Parameters controlling the DTU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsDTUbypass(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using Double Truncated Unary (DTU) CABAC decoding.
     * @param binParams Parameters controlling the DTU decoding.
     * @return The decoded symbol.
     */
    uint64_t readAsDTUcabac(const std::vector<unsigned int>& binParams);

    /**
     * @brief Reads a symbol using a Look-Up Table (LUT) for symbol decoding.
     * @param codingSubsymSize Size of the coding sub-symbol in bits.
     * @return The decoded symbol from the LUT.
     */
    uint64_t readLutSymbol(uint8_t codingSubsymSize);

    /**
     * @brief Reads a sign flag from the bitstream.
     * @return The sign flag (true for negative, false for positive).
     */
    bool readSignFlag();

    /**
     * @brief Initializes the Reader for a new bitstream.
     */
    void start();

    /**
     * @brief Closes the Reader and returns the number of bits read.
     * @return Number of bits read from the bitstream.
     */
    size_t close();

    /**
     * @brief Resets the Reader's state, clearing context models and stream.
     */
    void reset();

 private:
    BitInputStream m_bitInputStream;        //!< @brief Input bitstream for reading.
    BinaryArithmeticDecoder m_decBinCabac;  //!< @brief CABAC decoder for arithmetic coding.

    bool m_bypassFlag;       //!< @brief Indicates if bypass mode is enabled.
    uint64_t m_numContexts;  //!< @brief Number of contexts available for CABAC decoding.

    std::vector<ContextModel> m_contextModels;  //!< @brief Context models for CABAC decoding.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_READER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @brief Header file for the `Subsequence` class used in LZMA-based entropy decoding and encoding.
 * @details This file contains the `Subsequence` class, which is used to manage the configuration of
 *          subsequences in LZMA-based compression and decompression algorithms. It encapsulates
 *          information about the output symbol size of a subsequence and provides methods to read
 *          and write this configuration to bitstreams.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_LZMA_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::lzma {

/**
 * @brief Represents a subsequence configuration for LZMA-based entropy coding.
 * @details The `Subsequence` class is used to store and manage parameters related to a specific subsequence
 *          in LZMA-based entropy encoding and decoding. It encapsulates the output symbol size, and it provides
 *          methods for setting, getting, and serializing this value.
 */
class Subsequence {
 public:
    /**
     * @brief Construct a new Subsequence object with a specified output symbol size.
     * @param _output_symbol_size The size of each symbol in the subsequence, in bits.
     */
    explicit Subsequence(uint8_t _output_symbol_size);

    /**
     * @brief Retrieve the size of the output symbols in this subsequence.
     * @return The size of each output symbol in the subsequence, in bits.
     */
    [[nodiscard]] uint8_t getOutputSymbolSize() const;

    /**
     * @brief Write the subsequence configuration to a BitWriter.
     * @param writer Reference to the BitWriter object to serialize the configuration.
     */
    void write(util::BitWriter &writer) const;

    /**
     * @brief Equality comparison operator for the Subsequence class.
     * @param rhs Reference to the other `Subsequence` object to compare.
     * @return True if the `output_symbol_size` values are equal; otherwise, false.
     */
    bool operator==(const Subsequence &rhs) const;

 private:
    uint8_t output_symbol_size;  //!< @brief Size of each symbol in the subsequence, in bits.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

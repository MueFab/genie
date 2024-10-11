/**
 * @file
 * @brief This file contains the definition of the Subsequence class used for BSC entropy coding.
 * @details The Subsequence class represents a configuration for handling specific subsequence elements in a block-based entropy encoder.
 * @copyright This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Class representing a subsequence configuration for BSC entropy coding.
 */
class Subsequence {
 public:
    /**
     * @brief Construct a new Subsequence object with a specified output symbol size.
     * @param _output_symbol_size Size of the output symbols in bits.
     */
    explicit Subsequence(uint8_t _output_symbol_size);

    /**
     * @brief Get the output symbol size for this subsequence.
     * @return Output symbol size in bits.
     */
    [[nodiscard]] uint8_t getOutputSymbolSize() const;

    /**
     * @brief Write the subsequence configuration to a BitWriter.
     * @param writer BitWriter object to write the configuration to.
     */
    void write(util::BitWriter &writer) const;

    /**
     * @brief Compare this subsequence configuration to another for equality.
     * @param rhs Another Subsequence object to compare to.
     * @return True if both configurations are equal.
     */
    bool operator==(const Subsequence &rhs) const;

 private:
    uint8_t output_symbol_size;  //!< @brief Output symbol size in bits.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

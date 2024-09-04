/**
* @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

// ---------------------------------------------------------------------------------------------------------------------

class Subsequence {
 public:
    /**
     *
     * @param _output_symbol_size
     */
    explicit Subsequence(uint8_t _output_symbol_size);

    /**
     *
     * @return
     */
    [[nodiscard]] uint8_t getOutputSymbolSize() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const;

    /**
     * @brief
     * @param rhs
     * @return
     */
    bool operator==(const Subsequence &rhs) const;

 private:
    uint8_t output_symbol_size;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

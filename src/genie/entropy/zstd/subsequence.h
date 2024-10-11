/**
 * @file
 * @brief Header file defining the `Subsequence` class for the ZSTD entropy coding module.
 * @details This file is part of the ZSTD entropy parameter configuration and defines the `Subsequence` class,
 *          which represents a configuration unit used in ZSTD entropy coding/decoding processes. The class
 *          contains parameters specific to subsequence encoding, such as output symbol size. It provides
 *          serialization and equality checking functionalities.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_ZSTD_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::zstd {

/**
 * @brief Class representing a subsequence configuration for the ZSTD entropy coding module.
 * @details This class stores and manages the configuration for a subsequence in the ZSTD entropy coder, which
 *          is an integral part of descriptor-based compression within the MPEG-G codec.
 */
class Subsequence {
 public:
    /**
     * @brief Construct a new Subsequence object with a specified output symbol size.
     * @param _output_symbol_size Size of the output symbols in bytes.
     */
    explicit Subsequence(uint8_t _output_symbol_size);

    /**
     * @brief Get the output symbol size of the subsequence.
     * @return Output symbol size in bytes.
     */
    [[nodiscard]] uint8_t getOutputSymbolSize() const;

    /**
     * @brief Serialize the subsequence configuration to a BitWriter.
     * @param writer Reference to the BitWriter object to store the serialized data.
     */
    void write(util::BitWriter &writer) const;

    /**
     * @brief Compare this subsequence configuration with another for equality.
     * @param rhs Another Subsequence object to compare with.
     * @return True if both subsequence configurations are identical.
     */
    bool operator==(const Subsequence &rhs) const;

 private:
    uint8_t output_symbol_size;  //!< @brief Size of the output symbols in bytes.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

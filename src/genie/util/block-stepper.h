/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BLOCK_STEPPER_H_
#define SRC_GENIE_UTIL_BLOCK_STEPPER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief A handle to quickly iterate over a DataBlock.
 */
struct BlockStepper {
    uint8_t *curr;     //!< @brief Pointer to current position.
    uint8_t *end;      //!< @brief Pointer to the first invalid position.
    uint8_t wordSize;  //!< @brief Size of each element.

    /**
     * @brief Construct a new blockstepper from the raw data.
     * @param _cur Current position.
     * @param _end End position.
     * @param _wordSize Size in bytes of one element.
     */
    explicit BlockStepper(uint8_t *_cur = nullptr, uint8_t *_end = nullptr, uint8_t _wordSize = 1);

    /**
     * @return True, if the current position is not yet the end position.
     */
    bool isValid() const;

    /**
     * @brief Increment by one element.
     */
    void inc();

    /**
     * @return The current element.
     */
    uint64_t get() const;

    /**
     * @brief Set the current element to a new value.
     * @param val New value.
     */
    void set(uint64_t val) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/block-stepper.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BLOCK_STEPPER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

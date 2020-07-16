/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_BLOCK_STEPPER_H_
#define GABAC_BLOCK_STEPPER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
struct BlockStepper {
    uint8_t *curr;     //!<
    uint8_t *end;      //!<
    uint8_t wordSize;  //!<

    /**
     *
     * @param _cur
     * @param _end
     * @param _wordSize
     */
    explicit BlockStepper(uint8_t *_cur = nullptr, uint8_t *_end = nullptr, uint8_t _wordSize = 1);

    /**
     *
     * @return
     */
    bool isValid() const;

    /**
     *
     */
    void inc();

    /**
     *
     * @return
     */
    uint64_t get() const;

    /**
     *
     * @param val
     */
    void set(uint64_t val) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "block-stepper.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_BLOCK_STEPPER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
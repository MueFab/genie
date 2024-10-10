/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_
#define SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

inline bool BlockStepper::isValid() const { return curr != end; }

// ---------------------------------------------------------------------------------------------------------------------

inline void BlockStepper::inc() { curr += wordSize; }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t BlockStepper::get() const {
    switch (wordSize) {
        case 1:
            return *curr;
        case 2:
            return *reinterpret_cast<uint16_t *>(curr);
        case 4:
            return *reinterpret_cast<uint32_t *>(curr);
        case 8:
            return *reinterpret_cast<uint64_t *>(curr);
        default:
            break;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

inline void BlockStepper::set(uint64_t val) const {
    switch (wordSize) {
        case 1:
            *curr = static_cast<uint8_t>(val);
            return;
        case 2:
            *reinterpret_cast<uint16_t *>(curr) = static_cast<uint16_t>(val);
            return;
        case 4:
            *reinterpret_cast<uint32_t *>(curr) = static_cast<uint32_t>(val);
            return;
        case 8:
            *reinterpret_cast<uint64_t *>(curr) = static_cast<uint64_t>(val);
            return;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

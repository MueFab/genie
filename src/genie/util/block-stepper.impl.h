/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_
#define SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

inline bool BlockStepper::IsValid() const { return curr != end; }

// ---------------------------------------------------------------------------------------------------------------------

inline void BlockStepper::Inc() { curr += wordSize; }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t BlockStepper::Get() const {
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

inline void BlockStepper::Set(uint64_t val) const {
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

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

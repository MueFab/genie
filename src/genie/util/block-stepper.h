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
    explicit BlockStepper(uint8_t *_cur = nullptr, uint8_t *_end = nullptr, uint8_t _wordSize = 1)
        : curr(_cur), end(_end), wordSize(_wordSize) {}

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

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_BLOCK_STEPPER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
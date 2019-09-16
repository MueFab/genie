/**
 * @file
 * @brief Gabac fast data block traversal
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_BLOCK_STEPPER_H_
#define GABAC_BLOCK_STEPPER_H_

#include <cstdint>

namespace gabac {

/**
 * @brief Allows to traverse a DataBlock sequentially in a fast way
 * Blockstepper r = datablock.getreader();
 * while(r.isValid()) {auto v = r.get(); ... ; r.inc();}
 */
struct BlockStepper {
    uint8_t *curr;    /**< @brief Current element */
    uint8_t *end;     /**< @brief Last Element in the array */
    uint8_t wordSize; /**< @brief Bytes per element*/

    /**
     * @brief Check if curr == end
     * @return
     */
    bool isValid() const;

    /**
     * @brief Increment curr to point to the next element
     */
    void inc();

    /**
     * @brief Read current element
     * @return Value
     */
    uint64_t get() const;

    /**
     * @brief Set current element
     */
    void set(uint64_t val) const;
};

inline bool BlockStepper::isValid() const { return curr != end; }

inline void BlockStepper::inc() { curr += wordSize; }

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
}  // namespace gabac

#endif  // GABAC_BLOCK_STEPPER_H_

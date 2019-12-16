/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DRAIN_H
#define GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

/**
 *
 * @tparam TYPE
 */
template <typename TYPE>
class Drain {
   public:
    /**
     *
     * @param t
     * @param id
     */
    virtual void flowIn(TYPE t, size_t id) = 0;

    /**
     *
     */
    virtual void dryIn() = 0;

    /**
     *
     */
    virtual ~Drain() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

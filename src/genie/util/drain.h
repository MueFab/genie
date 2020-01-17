/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DRAIN_H
#define GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief By implementing this interface, a genie module signals the ability to process data of type TYPE.
 * @tparam TYPE Input data type which is accepted
 */
template <typename TYPE>
class Drain {
   public:
    /**
     * @brief Accept one chunk of data.
     * @param t Input data chunk
     * @param id Current block identifier (for multithreading)
     */
    virtual void flowIn(TYPE&& t, size_t id) = 0;

    /**
     * @brief Signals that no further data (and calls to flowIn) will be available
     */
    virtual void dryIn() = 0;

    /**
     * @brief For inheritance
     */
    virtual ~Drain() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DRAIN_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

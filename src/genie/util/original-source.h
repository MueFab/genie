/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ORIGINAL_SOURCE_H
#define GENIE_ORIGINAL_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <mutex>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Implementing this interface signals the ability to introduce new data into the application without any input
 */
class OriginalSource {
   public:
    /**
     * @brief Spawn some data
     * @param current block id
     * @return True if more data is available, false otherwise
     */
    virtual bool pump(size_t& id, std::mutex& lock) = 0;

    /**
     * @brief Signal end of data.
     */
    virtual void flushIn() = 0;

    /**
     * @brief For inheritance
     */
    virtual ~OriginalSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ORIGINAL_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
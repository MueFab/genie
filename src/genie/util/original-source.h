/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_
#define SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <mutex> //NOLINT

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Implementing this interface signals the ability to introduce new data into the application without any input
 */
class OriginalSource {
 public:
    /**
     * @brief
     * @param id
     * @param lock
     * @return
     */
    virtual bool pump(uint64_t& id, std::mutex& lock) = 0;

    /**
     * @brief Signal end of data.
     */
    virtual void flushIn(uint64_t& pos) = 0;

    /**
     * @brief For inheritance
     */
    virtual ~OriginalSource() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORIGINAL_SOURCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

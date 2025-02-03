/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ORDERED_SECTION_H_
#define SRC_GENIE_UTIL_ORDERED_SECTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Protects a section of code using an ordered lock
 */
class OrderedSection {
 private:
    OrderedLock* lock;  //!< @brief Underlying lock
    size_t length;      //!< @brief

 public:
    /**
     * @brief Try to acquire the lock
     * @param _lock Underlying lock
     * @param id Block identifier to wait for
     */
    OrderedSection(OrderedLock* _lock, const Section& id);

    /**
     * @brief Release the lock
     */
    ~OrderedSection();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ORDERED_SECTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

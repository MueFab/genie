/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ORDERED_SECTION_H
#define GENIE_ORDERED_SECTION_H

// ---------------------------------------------------------------------------------------------------------------------

#include "ordered-lock.h"

namespace util {

/**
 * @brief Protects a section of code using an ordered lock
 */
class OrderedSection {
   private:
    OrderedLock* lock;  //!< @brief Underlying lock

   public:
    /**
     * @brief Try to aquire the lock
     * @param _lock Underlying lock
     * @param id Block identifier to wait for
     */
    OrderedSection(OrderedLock* _lock, size_t id);

    /**
     * @brief Release the lock
     */
    ~OrderedSection();
};

}  // namespace util
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ORDERED_SECTION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
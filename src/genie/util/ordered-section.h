/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ORDERED_SECTION_H
#define GENIE_ORDERED_SECTION_H

// ---------------------------------------------------------------------------------------------------------------------

#include "drain.h"
#include "ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Protects a section of code using an ordered lock
 */
class OrderedSection {
   private:
    OrderedLock* lock;  //!< @brief Underlying lock
    size_t length;

   public:
    /**
     * @brief Try to aquire the lock
     * @param _lock Underlying lock
     * @param id Block identifier to wait for
     */
    OrderedSection(OrderedLock* _lock, const Section& id);

    void setLength(size_t l) { length = l; }

    /**
     * @brief Release the lock
     */
    ~OrderedSection();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ORDERED_SECTION_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
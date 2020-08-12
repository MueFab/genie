/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SOURCE_H
#define GENIE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include "drain.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Implementing this interface signals the ability to output data chunks of type TYPE.
 * @tparam TYPE Data chunk type.
 */
template <typename TYPE>
class Source {
   protected:
    Drain<TYPE>* drain;  //!< @brief Where to propagate output to.

   protected:
    /**
     * @brief Propagate data to drain set before.
     * @param t Current chunk of data.
     * @param id Block identifier (for multithreading).
     */
    void flowOut(TYPE&& t, const Section& id);

    /**
     * @brief Propagates end-of-data signal to drain set before.
     */
    void flushOut(size_t& pos);

    /**
     * @brief Propagates skip signal to drain set before.
     * @param id Section to skip.
     */
    void skipOut(const Section& id);

   public:
    /**
     * @brief Current data output will be propagated to a new destination.
     * @param d New destination for output data.
     */
    virtual void setDrain(Drain<TYPE>* d);

    /**
     * @brief For inheritance.
     */
    virtual ~Source() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "source.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
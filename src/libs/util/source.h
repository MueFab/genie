/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SOURCE_H
#define GENIE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include "drain.h"

/**
 *
 * @tparam TYPE
 */
template <typename TYPE>
class Source {
   private:
    Drain<TYPE>* drain;  //!<

   protected:
    /**
     *
     * @param t
     * @param id
     */
    void flowOut(TYPE t, size_t id) { drain->flowIn(std::move(t), id); }

    /**
     *
     */
    void dryOut() { drain->dryIn(); }

   public:
    /**
     *
     * @param d
     */
    virtual void setDrain(Drain<TYPE>* d) { drain = d; }

    /**
     *
     * @return
     */
    virtual bool pump() { return false; }

    /**
     *
     */
    virtual ~Source() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SOURCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_H
#define GENIE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>

#include "drain.h"
#include "selector-head.h"
#include "selector-tail.h"
#include "source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam Tin
 * @tparam Tout
 */
template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin>, public genie::util::Source<Tout> {
   private:
    SelectorHead<Tin> head;   //!<
    SelectorTail<Tout> tail;  //!<

   public:
    /**
     *
     * @param select
     */
    explicit Selector(std::function<size_t(const Tin& t)> select);

    /**
     *
     */
    Selector() = default;

    /**
     *
     */
    void addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out);

    /**
     *
     */
    void setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index);

    /**
     *
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(Tin&& t, const util::Section& id) override;

    /**
     *
     * @param pos
     */
    void flushIn(size_t& pos) override;

    /**
     *
     * @param id
     */
    void skipIn(const Section& id) override;

    /**
     *
     */
    void setDrain(genie::util::Drain<Tout>* d) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
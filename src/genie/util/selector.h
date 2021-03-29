/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_H_
#define SRC_GENIE_UTIL_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>
#include "genie/util/drain.h"
#include "genie/util/selector-head.h"
#include "genie/util/selector-tail.h"
#include "genie/util/source.h"

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
    void flushIn(uint64_t& pos) override;

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

#include "genie/util/selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

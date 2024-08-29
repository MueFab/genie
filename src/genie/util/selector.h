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

namespace genie::util {

/**
 * @brief
 * @tparam Tin
 * @tparam Tout
 */
template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin>, public genie::util::Source<Tout> {
 private:
    SelectorHead<Tin> head;   //!< @brief
    SelectorTail<Tout> tail;  //!< @brief

 public:
    /**
     * @brief
     */
    Selector() = default;

    /**
     * @brief
     */
    void addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out);

    /**
     * @brief
     */
    void setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index);

    /**
     * @brief
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s);

    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(Tin&& t, const util::Section& id) override;

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief
     * @param id
     */
    void skipIn(const Section& id) override;

    /**
     * @brief
     */
    void setDrain(genie::util::Drain<Tout>* d) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

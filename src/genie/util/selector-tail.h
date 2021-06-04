/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_TAIL_H_
#define SRC_GENIE_UTIL_SELECTOR_TAIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam Tout
 */
template <typename Tout>
class SelectorTail : public genie::util::Source<Tout>, public genie::util::Drain<Tout> {
 private:
    size_t dryCtr{};    //!<
    size_t modNum{};    //!<
    size_t position{};  //!<

 public:
    /**
     *
     */
    SelectorTail();

    /**
     *
     */
    void addMod();

    /**
     *
     * @param num
     */
    void setNum(size_t num);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(Tout&& t, const Section& id) override;

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
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/selector-tail.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_TAIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

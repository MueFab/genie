/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_TAIL_H
#define GENIE_SELECTOR_TAIL_H

// ---------------------------------------------------------------------------------------------------------------------

#include "source.h"

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
    explicit SelectorTail();

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
    void flushIn(size_t& pos) override;

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

#include "selector-tail.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_TAIL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
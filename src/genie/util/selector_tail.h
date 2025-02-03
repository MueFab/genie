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
 * @brief
 * @tparam Tout
 */
template <typename Tout>
class SelectorTail : public genie::util::Source<Tout>, public genie::util::Drain<Tout> {
 private:
    size_t dryCtr{};    //!< @brief
    size_t modNum{};    //!< @brief
    size_t position{};  //!< @brief

 public:
    /**
     * @brief
     */
    SelectorTail();

    /**
     * @brief
     */
    void AddMod();

    /**
     * @brief
     * @param num
     */
    void setNum(size_t num);

    /**
     * @brief
     * @param t
     * @param id
     */
    void FlowIn(Tout&& t, const Section& id) override;

    /**
     * @brief
     * @param pos
     */
    void FlushIn(uint64_t& pos) override;

    /**
     * @brief
     * @param id
     */
    void SkipIn(const Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/selector_tail.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_TAIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_
#define SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
SelectorTail<Tout>::SelectorTail() : modNum(0), position(0) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::AddMod() {
    modNum++;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::setNum(size_t num) {
    modNum = num;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::FlowIn(Tout&& t, const Section& id) {
    genie::util::Source<Tout>::FlowOut(std::move(t), id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::FlushIn(uint64_t& pos) {
    size_t num = ++dryCtr;
    if (num == modNum) {
        // Output signal once every module contained finished
        dryCtr = 0;
        genie::util::Source<Tout>::FlushOut(pos);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::SkipIn(const Section& id) {
    if (id.strongSkip) {
        genie::util::Source<Tout>::SkipOut(id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_TAIL_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_TAIL_IMPL_H
#define GENIE_SELECTOR_TAIL_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
SelectorTail<Tout>::SelectorTail() : modNum(0), position(0) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::addMod() {
    modNum++;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::setNum(size_t num) {
    modNum = num;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::flowIn(Tout&& t, const Section& id) {
    genie::util::Source<Tout>::flowOut(std::move(t), id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::flushIn(size_t& pos) {
    size_t num = ++dryCtr;
    if (num == modNum) {
        // Output signal once every module contained finished
        dryCtr = 0;
        genie::util::Source<Tout>::flushOut(pos);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tout>
void SelectorTail<Tout>::skipIn(const Section& id) {
    if (id.strongSkip) {
        genie::util::Source<Tout>::skipOut(id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_TAIL_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
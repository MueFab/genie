/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_IMPL_H_
#define SRC_GENIE_UTIL_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::AddBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out) {
    head.add(entry);
    tail.AddMod();
    out->SetDrain(&tail);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index) {
    head.set(entry, index);
    out->SetDrain(&tail);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetOperation(std::function<size_t(const Tin& t)> s) {
    head.setOperation(s);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::FlowIn(Tin&& t, const util::Section& id) {
    head.FlowIn(std::move(t), id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::FlushIn(uint64_t& pos) {
    head.FlushIn(pos);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SkipIn(const Section& id) {
    head.SkipIn(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::SetDrain(genie::util::Drain<Tout>* d) {
    tail.SetDrain(d);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

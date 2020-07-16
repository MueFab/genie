/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_IMPL_H
#define GENIE_SELECTOR_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
Selector<Tin, Tout>::Selector(std::function<size_t(const Tin& t)> select) : head(select) {
    head.setTail(&tail);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out) {
    head.add(entry);
    tail.addMod();
    out->setDrain(&tail);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index) {
    head.set(entry, index);
    out->setDrain(&tail);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::setOperation(std::function<size_t(const Tin& t)> s) {
    head.setOperation(s);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::flowIn(Tin&& t, const util::Section& id) {
    head.flowIn(std::move(t), id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::flushIn(size_t& pos) {
    head.flushIn(pos);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::skipIn(const Section& id) {
    head.skipIn(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin, typename Tout>
void Selector<Tin, Tout>::setDrain(genie::util::Drain<Tout>* d) {
    tail.setDrain(d);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_HEAD_IMPL_H
#define GENIE_SELECTOR_HEAD_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::route(Tin&& t, size_t mod_id, Section global_id) {
    for (size_t i = 0; i < mods.size(); ++i) {
        if (i == mod_id) {
            mods[i]->flowIn(std::move(t), global_id);
        } else {
            mods[i]->skipIn(Section{global_id.start, global_id.length, false});
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
size_t SelectorHead<Tin>::defaultSelect(const Tin&) {
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
SelectorHead<Tin>::SelectorHead() : select(&defaultSelect) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
SelectorHead<Tin>::SelectorHead(std::function<size_t(const Tin& t)> _selector) : select(_selector) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::add(genie::util::Drain<Tin>* mod) {
    mods.push_back(mod);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::set(genie::util::Drain<Tin>* mod, size_t index) {
    mods[index] = mod;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::setOperation(std::function<size_t(const Tin& t)> s) {
    select = s;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::flowIn(Tin&& t, const Section& id) {
    Tin in = std::move(t);
    size_t dest = select(in);
    route(std::move(in), dest, id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::flushIn(size_t& pos) {
    for (const auto& m : mods) {
        m->flushIn(pos);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::skipIn(const Section& id) {
    bool first = true;
    for (const auto& m : mods) {
        if (first) {
            m->skipIn(id);
            first = false;
        } else {
            Section locID = id;
            locID.strongSkip = false;
            m->skipIn(locID);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_HEAD_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
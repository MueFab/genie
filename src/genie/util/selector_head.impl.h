/**
 * Copyright 2018-2024 The Genie Authors.
 * @file selector_head.impl.h
 * @brief Implementation of the SelectorHead class template for routing data to
 * different modules.
 *
 * This file contains the implementation of the SelectorHead class template,
 * which is responsible for routing data to different modules based on a
 * selection function. It includes methods for adding and setting modules,
 * defining a custom selection function, and managing the flow of data through
 * the modules.
 *
 * @details The SelectorHead class template provides mechanisms to route data to
 * the appropriate module, Flush data through all modules, and skip sections of
 * data. It supports operations such as adding modules, setting a specific
 * module at an index, and defining a custom selection function to determine the
 * target module for the data.
 *
 * @tparam Tin The type of the data being routed to the modules.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_HEAD_IMPL_H_
#define SRC_GENIE_UTIL_SELECTOR_HEAD_IMPL_H_

// -----------------------------------------------------------------------------

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::Route(Tin&& t, const size_t mod_id, Section global_id) {
  for (size_t i = 0; i < mods_.size(); ++i) {
    if (i == mod_id) {
      mods_[i]->FlowIn(std::move(t), global_id);
    } else {
      mods_[i]->SkipIn(Section{global_id.start, global_id.length, false});
    }
  }
}

// -----------------------------------------------------------------------------

template <typename Tin>
size_t SelectorHead<Tin>::DefaultSelect(const Tin&) {
  return 0;
}

// -----------------------------------------------------------------------------

template <typename Tin>
SelectorHead<Tin>::SelectorHead() : select_(&DefaultSelect) {}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::Add(Drain<Tin>* mod) {
  mods_.push_back(mod);
}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::Set(Drain<Tin>* mod, size_t index) {
  mods_[index] = mod;
}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::SetOperation(std::function<size_t(const Tin& t)> s) {
  select_ = s;
}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::FlowIn(Tin&& t, const Section& id) {
  Tin in = std::move(t);
  const size_t dest = select_(in);
  Route(std::move(in), dest, id);
}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::FlushIn(uint64_t& pos) {
  for (const auto& m : mods_) {
    m->FlushIn(pos);
  }
}

// -----------------------------------------------------------------------------

template <typename Tin>
void SelectorHead<Tin>::SkipIn(const Section& id) {
  bool first = true;
  for (const auto& m : mods_) {
    if (first) {
      m->SkipIn(id);
      first = false;
    } else {
      Section loc_id = id;
      m->SkipIn(loc_id);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_HEAD_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

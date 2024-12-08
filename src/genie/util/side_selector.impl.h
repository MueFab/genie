/**
 * Copyright 2018-2024 The Genie Authors.
 * @file side_selector.impl.h
 * @brief Implementation of the SideSelector class template for selecting and
 * processing with different coders.
 *
 * This file contains the implementation of the SideSelector class template,
 * which allows for selecting and processing with different coder modules based
 * on a selection function. It supports setting and adding coder modules, as
 * well as defining a custom selection function.
 *
 * @details The SideSelector class template provides methods for setting a coder
 * module at a specific index, adding a new coder module, setting a custom
 * selection function, and processing with the selected coder module. The
 * default selection function returns the first coder module.
 *
 * @tparam Coder The type of the coder modules.
 * @tparam Ret The return type of the process function.
 * @tparam Args The argument types for the process function.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_
#define SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
size_t SideSelector<Coder, Ret, Args...>::DefaultSelect(Args...) {
  return 0;
}

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
SideSelector<Coder, Ret, Args...>::SideSelector() : select_(&DefaultSelect) {}

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::SetMod(Coder* mod, size_t index) {
  mods_[index] = mod;
}

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::AddMod(Coder* mod) {
  mods_.emplace_back(mod);
}

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::SetSelection(
    std::function<size_t(Args...)> select) {
  select_ = select;
}

// -----------------------------------------------------------------------------
template <typename Coder, typename Ret, typename... Args>
Ret SideSelector<Coder, Ret, Args...>::Process(Args... param) {
  size_t index = select_(param...);
  UTILS_DIE_IF(index >= mods_.size(), "Invalid index in SideSelector");
  return mods_[index]->Process(param...);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

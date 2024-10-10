/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_
#define SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
size_t SideSelector<Coder, Ret, Args...>::defaultSelect(Args...) {
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
SideSelector<Coder, Ret, Args...>::SideSelector() : select(&defaultSelect) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::setMod(Coder* mod, size_t index) {
    mods[index] = mod;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::addMod(Coder* mod) {
    mods.emplace_back(mod);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::setSelection(std::function<size_t(Args...)> _select) {
    select = _select;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
Ret SideSelector<Coder, Ret, Args...>::process(Args... param) {
    size_t index = select(param...);
    UTILS_DIE_IF(index >= mods.size(), "Invalid index in SideSelector");
    return mods[index]->process(param...);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

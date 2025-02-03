/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_
#define SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
size_t SideSelector<Coder, Ret, Args...>::defaultSelect(Args...) {
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
SideSelector<Coder, Ret, Args...>::SideSelector() : select_(&defaultSelect) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::SetMod(Coder* mod, size_t index) {
    mods[index] = mod;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::AddMod(Coder* mod) {
    mods.emplace_back(mod);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
void SideSelector<Coder, Ret, Args...>::SetSelection(std::function<size_t(Args...)> _select) {
    select_ = _select;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
Ret SideSelector<Coder, Ret, Args...>::Process(Args... param) {
    size_t index = select_(param...);
    UTILS_DIE_IF(index >= mods.size(), "Invalid index in SideSelector");
    return mods[index]->Process(param...);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SIDE_SELECTOR_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

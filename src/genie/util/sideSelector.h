/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SIDESELECTOR_H
#define GENIE_SIDESELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename Coder, typename Ret, typename... Args>
class SideSelector {
   private:
    std::vector<Coder*> mods; //!<
    std::function<size_t(Args...)> select; //!<

    /**
     *
     * @param ...
     * @return
     */
    static size_t defaultSelect(Args...) { return 0; }

   public:
    /**
     *
     */
    SideSelector() : select(&defaultSelect) {}

    /**
     *
     * @param mod
     * @param index
     */
    void setMod(Coder* mod, size_t index) { mods[index] = mod; }

    /**
     *
     * @param mod
     */
    void addMod(Coder* mod) { mods.emplace_back(mod); }

    /**
     *
     * @param _select
     */
    void setSelection(std::function<size_t(Args...)> _select) { select = _select; }

    /**
     *
     * @param param
     * @return
     */
    Ret process(Args... param) {
        size_t index = select(param...);
        UTILS_DIE_IF(index >= mods.size(), "Invalid index in SideSelector");
        return mods[index]->process(param...);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIDESELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
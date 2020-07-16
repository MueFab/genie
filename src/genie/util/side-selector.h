/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SIDE_SELECTOR_H
#define GENIE_SIDE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include "runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam Coder
 * @tparam Ret
 * @tparam Args
 */
template <typename Coder, typename Ret, typename... Args>
class SideSelector {
   private:
    std::vector<Coder*> mods;               //!<
    std::function<size_t(Args...)> select;  //!<

    /**
     *
     * @param ...
     * @return
     */
    static size_t defaultSelect(Args...);

   public:
    /**
     *
     */
    SideSelector();

    /**
     *
     * @param mod
     * @param index
     */
    void setMod(Coder* mod, size_t index);

    /**
     *
     * @param mod
     */
    void addMod(Coder* mod);

    /**
     *
     * @param _select
     */
    void setSelection(std::function<size_t(Args...)> _select);

    /**
     *
     * @param param
     * @return
     */
    Ret process(Args... param);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "side-selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SIDE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
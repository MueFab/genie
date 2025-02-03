/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SIDE_SELECTOR_H_
#define SRC_GENIE_UTIL_SIDE_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <vector>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Redirects input data to a module which is selected by a provided function.
 * @tparam Coder Interface for the modules.
 * @tparam Ret Return type for the module process function.
 * @tparam Args Parameter types for the module process function.
 */
template <typename Coder, typename Ret, typename... Args>
class SideSelector {
 private:
    std::vector<Coder*> mods;               //!< @brief List of possible modules to select.
    std::function<size_t(Args...)> select;  //!< @brief select function. It returns the index of the selected module.

    /**
     * @brief A default selection function.
     * @param ... Parameters required by the module interface.
     * @return For this default selector always 0.
     */
    static size_t defaultSelect(Args...);

 public:
    /**
     * @brief Default initialization.
     */
    SideSelector();

    /**
     * @brief Set the module at a specified index.
     * @param mod The new module.
     * @param index Index where to insert the module.
     * @attention The index has to be added by addMod() before.
     */
    void setMod(Coder* mod, size_t index);

    /**
     * @brief Add a new module to the end of the module list.
     * @param mod The new module.
     */
    void addMod(Coder* mod);

    /**
     * @brief Set the selection function which will determine to which module data is passed.
     * @param _select A selection function returning an index.
     */
    void setSelection(std::function<size_t(Args...)> _select);

    /**
     * @brief
     * @param param
     * @return
     */
    Ret process(Args... param);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/side_selector.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SIDE_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

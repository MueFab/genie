/**
 * Copyright 2018-2024 The Genie Authors.
 * @file side_selector.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the SideSelector class template for dynamically
 * selecting modules based on input parameters.
 *
 * This file contains the declaration of the `SideSelector` class template,
 * which provides a mechanism for redirecting input data to one of several
 * possible modules based on a selection function. It allows for dynamic
 * selection of a module at runtime, using a function that determines which
 * module should process the given input parameters.
 *
 * @details The `SideSelector` class template is useful for implementing
 * branching logic within a data processing pipeline. It supports adding new
 * modules, setting a custom selection function, and invoking the selected
 * module based on runtime parameters. The class is parameterized by the module
 * interface type
 * (`Coder`), the return type (`Ret`), and the parameter types (`Args...`).
 */

#ifndef SRC_GENIE_UTIL_SIDE_SELECTOR_H_
#define SRC_GENIE_UTIL_SIDE_SELECTOR_H_

// -----------------------------------------------------------------------------

#include <functional>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Dynamically redirects input data to a selected module based on a
 * provided function.
 *
 * The `SideSelector` class template allows for dynamic selection of a
 * processing module based on runtime parameters. It uses a selection function
 * to determine which module should handle the input data. The class supports
 * adding new modules, setting the selection function, and invoking the selected
 * module using the `process()` method.
 *
 * @tparam Coder The interface type for the modules.
 * @tparam Ret The return type for the module's processing function.
 * @tparam Args The parameter types for the module's processing function.
 */
template <typename Coder, typename Ret, typename... Args>
class SideSelector {
  std::vector<Coder*>
      mods_;  //!< @brief List of available modules for selection.
  std::function<size_t(Args...)>
      select_;  //!< @brief Selection function that returns the index of the
                //!< selected module.

  /**
   * @brief Default selection function for the `SideSelector`.
   *
   * This function serves as the default selection logic when no custom
   * function is provided. It always returns zero, meaning that the first
   * module in the list will be used.
   *
   * @param ... Parameters required by the module interface (unused).
   * @return The index of the selected module (always zero for the default
   * selector).
   */
  static size_t DefaultSelect(Args...);

 public:
  /**
   * @brief Constructs a `SideSelector` object with the default selection
   * function.
   *
   * Initializes an empty `SideSelector` with no modules and the default
   * selection function. The selection function can be changed using the
   * `setSelection()` method.
   */
  SideSelector();

  /**
   * @brief Sets a specific module at the given index.
   *
   * This function updates the module at the specified index in the module
   * list. It is used to assign a new module to an existing index, replacing
   * the previous module. The index must already exist in the list, which
   * means `addMod()` should be used to add modules before setting them.
   *
   * @param mod Pointer to the module to be set at the specified index.
   * @param index The index in the module list where the new module should be
   * set.
   *
   * @attention The index must be valid and added using `addMod()` before
   * calling this function.
   */
  void SetMod(Coder* mod, size_t index);

  /**
   * @brief Adds a new module to the end of the module list.
   *
   * This function appends a new module to the list of available modules in
   * the selector. It expands the list to include the new module and assigns
   * it the next available index.
   *
   * @param mod Pointer to the new module to be added.
   */
  void AddMod(Coder* mod);

  /**
   * @brief Sets the custom selection function for choosing the module.
   *
   * This function updates the selection logic used to determine which module
   * should handle the input data. The selection function takes the input
   * parameters (`Args...`) and returns the index of the selected module.
   *
   * @param select The custom selection function to be used for module
   * selection.
   */
  void SetSelection(std::function<size_t(Args...)> select);

  /**
   * @brief Processes the input data using the selected module.
   *
   * This function calls the processing function of the selected module based
   * on the current selection function. It forwards the input parameters to
   * the selected module and returns the result of the module's processing
   * function.
   *
   * @param param The input parameters to be forwarded to the selected module.
   * @return The result of the selected module's processing function.
   *
   * @throws RuntimeException if the selected module index is out of range or
   * not set.
   */
  Ret Process(Args... param);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/side_selector.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SIDE_SELECTOR_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

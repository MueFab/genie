/**
 * @file module-manager.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the detect function for managing GENIE modules.
 *
 * This file contains the declaration of the `detect` function, which is part of the `module` namespace
 * within the GENIE framework. The `detect` function is used to scan and detect available modules in the
 * GENIE system, enabling dynamic configuration and integration of different components.
 *
 * @details The module manager handles the detection of various modules that can be dynamically loaded
 * or used within the GENIE framework. It is a central utility that helps identify which modules are
 * available for use, ensuring that GENIE components can adapt to different configurations.
 */

#ifndef SRC_GENIE_MODULE_MANAGER_H_
#define SRC_GENIE_MODULE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::module {

/**
 * @brief Detects and registers available GENIE modules.
 *
 * This function scans the environment or configuration files to identify and register
 * available GENIE modules. It enables dynamic configuration of the GENIE framework,
 * allowing it to adapt based on the modules that are detected. This is useful for integrating
 * different components or extensions into the system.
 *
 * @details The detect function can be used at initialization to determine which modules
 * are available for use. It helps configure the framework dynamically based on the detected
 * modules, making GENIE adaptable to different setups and environments.
 */
void detect();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::module

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_MODULE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

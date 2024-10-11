/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the ContextSelector class for adaptive context management in GABAC.
 *
 * This file contains the declaration of the `ContextSelector` class, which provides
 * methods to determine context indices for entropy encoding and decoding in the GABAC
 * (Genomic Adaptive Binary Arithmetic Coding) framework. The context indices are calculated
 * based on symbol and subsequence information to improve the efficiency of the arithmetic
 * coding process.
 *
 * @details The ContextSelector class uses the state variables defined in the CABAC framework
 * to derive context indices for both order-0 and higher-order contexts. These context indices
 * are then used to select appropriate probability models during entropy encoding and decoding.
 * The class interacts with other GABAC modules, including the subsymbol and state variables
 * definitions, to compute context indices for the various symbol streams.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>                                // For runtime assertions to validate input values.
#include <vector>                                 // For handling vectors of subsymbols.
#include "genie/entropy/gabac/context-tables.h"   // For building context tables.
#include "genie/entropy/gabac/subsymbol.h"        // For the Subsymbol struct definition.
#include "genie/entropy/paramcabac/state_vars.h"  // For handling state variables during context selection.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Class to handle context selection for adaptive entropy coding.
 *
 * The `ContextSelector` class is responsible for managing and selecting context indices
 * based on the current state of the encoding process. It supports both order-0 and higher-order
 * context selections, taking into account previously seen subsymbols and other state variables.
 * The context indices generated are used to identify specific probability models in CABAC.
 */
class ContextSelector {
 public:
    /**
     * @brief Constructs a `ContextSelector` using the provided state variables.
     *
     * Initializes a `ContextSelector` with state variables that define the properties
     * of the coding scheme, such as the number of contexts and symbols. These state
     * variables are used to compute context indices during the encoding and decoding phases.
     *
     * @param _stateVars A reference to a `StateVars` object containing configuration
     *                   parameters for context management.
     */
    explicit ContextSelector(const paramcabac::StateVars& _stateVars);

    /**
     * @brief Copy constructor for `ContextSelector`.
     *
     * Creates a copy of an existing `ContextSelector` object, duplicating its state variables
     * and other internal settings. Useful for scenarios where multiple context selectors
     * need to operate independently.
     *
     * @param src The `ContextSelector` object to copy.
     */
    ContextSelector(const ContextSelector& src);

    /**
     * @brief Default destructor.
     *
     * The `ContextSelector` destructor ensures that any resources held by the object
     * are properly released. Since the class does not hold any dynamically allocated
     * resources, the default destructor is sufficient.
     */
    ~ContextSelector() = default;

    /**
     * @brief Computes the context index for order-0 coding.
     *
     * This method returns the context index for order-0 coding, where the context is determined
     * solely based on the index of the subsymbol. This is typically used when no dependency on
     * previous symbols is required.
     *
     * @param subsymIdx The index of the subsymbol for which the context index is required.
     * @return The context index for the specified subsymbol index.
     */
    unsigned int getContextIdxOrder0(uint8_t subsymIdx);

    /**
     * @brief Computes the context index for higher-order coding.
     *
     * This method calculates the context index for coding schemes that depend on previous symbols.
     * It takes into account the specified previous index (`prvIdx`) and a vector of subsymbols
     * (`subsymbols`) to derive a unique context index.
     *
     * @param subsymIdx The index of the current subsymbol.
     * @param prvIdx The index of the previous subsymbol to consider in context selection.
     * @param subsymbols A reference to a vector of `Subsymbol` objects representing
     *                   the subsymbols encountered so far.
     * @param codingOrder The coding order to determine the level of dependency.
     * @return The context index based on the current and previous subsymbols.
     */
    unsigned int getContextIdxOrderGT0(uint8_t subsymIdx, uint8_t prvIdx, const std::vector<Subsymbol>& subsymbols,
                                       uint8_t codingOrder);

 private:
    const paramcabac::StateVars stateVars;  //!< State variables used for context selection.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_SELECTOR_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

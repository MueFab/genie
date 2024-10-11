/**
 * @file
 * @brief Header file for the `Subsymbol` struct used in GABAC entropy encoding.
 * @details The `Subsymbol` struct represents a unit of coding within the GABAC framework. It stores information
 *          related to individual subsymbols used in the entropy coding process, such as subsymbol index, value,
 *          and lookup table parameters. This struct is typically utilized in conjunction with CABAC/GABAC encoders
 *          to track the state of each coded unit and manage context modeling based on previous values.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_
#define SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include "genie/entropy/gabac/context-tables.h"
#include "genie/entropy/paramcabac/state_vars.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Represents a subsymbol unit for GABAC entropy encoding.
 * @details Each subsymbol is a basic unit of coding information used within the GABAC framework. This structure
 *          maintains essential details for context modeling and lookup table-based encoding/decoding. The subsymbol
 *          value and its index are crucial for managing context-dependent coding schemes and performing transformations
 *          based on previous subsymbols' values.
 */
struct Subsymbol {
    uint8_t subsymIdx = 0;           //!< Index of the subsymbol within a symbol sequence.
    uint64_t subsymValue = 0;        //!< Value of the current subsymbol being processed.
    uint64_t prvValues[2] = {0, 0};  //!< Previous subsymbol values used for context modeling and prediction.
    uint64_t lutNumMaxElems = 0;     //!< Maximum number of elements in the lookup table (LUT) for this subsymbol.
    uint64_t lutEntryIdx = 0;        //!< Index of the entry in the LUT used for current subsymbol coding.

    /**
     * @brief Default constructor for the `Subsymbol` struct.
     * @details Initializes the subsymbol fields to zero values, providing a clean slate for subsequent assignments.
     */
    Subsymbol() = default;

    /**
     * @brief Constructor for the `Subsymbol` struct with initialization.
     * @param idx Index of the subsymbol.
     * @param value Value of the subsymbol.
     * @param prev1 First previous value for context modeling.
     * @param prev2 Second previous value for context modeling.
     * @param lutMaxElems Maximum elements in the lookup table for this subsymbol.
     * @param lutIdx Lookup table index for this subsymbol.
     */
    Subsymbol(uint8_t idx, uint64_t value, uint64_t prev1, uint64_t prev2, uint64_t lutMaxElems, uint64_t lutIdx)
        : subsymIdx(idx), subsymValue(value), lutNumMaxElems(lutMaxElems), lutEntryIdx(lutIdx) {
        prvValues[0] = prev1;
        prvValues[1] = prev2;
    }

    /**
     * @brief Equality comparison operator for `Subsymbol`.
     * @param other The `Subsymbol` instance to compare against.
     * @return `true` if all fields of both subsymbols match; `false` otherwise.
     */
    bool operator==(const Subsymbol& other) const {
        return subsymIdx == other.subsymIdx && subsymValue == other.subsymValue &&
               prvValues[0] == other.prvValues[0] && prvValues[1] == other.prvValues[1] &&
               lutNumMaxElems == other.lutNumMaxElems && lutEntryIdx == other.lutEntryIdx;
    }

    /**
     * @brief Resets the subsymbol values to their default states.
     * @details Sets all member variables to zero, effectively clearing the subsymbol state.
     */
    void reset() {
        subsymIdx = 0;
        subsymValue = 0;
        prvValues[0] = 0;
        prvValues[1] = 0;
        lutNumMaxElems = 0;
        lutEntryIdx = 0;
    }

    /**
     * @brief Prints the details of the subsymbol for debugging purposes.
     * @details Outputs the current state of the subsymbol to the console.
     */
    void printDebug() const {
        std::cout << "Subsymbol Index: " << static_cast<int>(subsymIdx)
                  << "\nSubsymbol Value: " << subsymValue
                  << "\nPrevious Values: [" << prvValues[0] << ", " << prvValues[1] << "]"
                  << "\nLUT Max Elements: " << lutNumMaxElems
                  << "\nLUT Entry Index: " << lutEntryIdx << std::endl;
    }
};

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_SUBSYMBOL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Header file for the `Subsymbol` struct used in GABAC entropy encoding.
 * @details The `Subsymbol` struct represents a unit of coding within the GABAC
 * framework. It stores information related to individual subsymbols used in the
 * entropy coding process, such as subsymbol index, value, and lookup table
 * parameters. This struct is typically utilized in conjunction with CABAC/GABAC
 * encoders to track the state of each coded unit and manage context modeling
 * based on previous values.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_SUB_SYMBOL_H_
#define SRC_GENIE_ENTROPY_GABAC_SUB_SYMBOL_H_

// -----------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <string>
#include "genie/entropy/gabac/context_tables.h"
#include "genie/entropy/paramcabac/state_vars.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "GABAC";

namespace genie::entropy::gabac {

/**
 * @brief Represents a subsymbol unit for GABAC entropy encoding.
 * @details Each subsymbol is a basic unit of coding information used within the
 * GABAC framework. This structure maintains essential details for context
 * modeling and lookup table-based encoding/decoding. The subsymbol value and
 * its index are crucial for managing context-dependent coding schemes and
 * performing transformations based on previous subsymbols' values.
 */
struct Subsymbol {
  uint8_t subsym_idx = 0;  //!< Index of the subsymbol within a symbol sequence.
  uint64_t subsym_value =
      0;  //!< Value of the current subsymbol being processed.
  uint64_t prv_values[2] = {0, 0};  //!< Previous subsymbol values used for
                                    //!< context modeling and prediction.
  uint64_t lut_num_max_elems = 0;   //!< Maximum number of elements in the
                                    //!< lookup table (LUT) for this subsymbol.
  uint64_t lut_entry_idx = 0;       //!< Index of the entry in the LUT used for
                                    //!< current subsymbol coding.

  /**
   * @brief Default constructor for the `Subsymbol` struct.
   * @details Initializes the subsymbol fields to zero values, providing a
   * clean slate for subsequent assignments.
   */
  Subsymbol() = default;

  /**
   * @brief Constructor for the `Subsymbol` struct with initialization.
   * @param idx Index of the subsymbol.
   * @param value Value of the subsymbol.
   * @param prev1 First previous value for context modeling.
   * @param prev2 Second previous value for context modeling.
   * @param lut_max_elems Maximum elements in the lookup table for this
   * subsymbol.
   * @param lut_idx Lookup table index for this subsymbol.
   */
  Subsymbol(uint8_t idx, uint64_t value, uint64_t prev1, uint64_t prev2,
            uint64_t lut_max_elems, uint64_t lut_idx)
      : subsym_idx(idx),
        subsym_value(value),
        lut_num_max_elems(lut_max_elems),
        lut_entry_idx(lut_idx) {
    prv_values[0] = prev1;
    prv_values[1] = prev2;
  }

  /**
   * @brief Equality comparison operator for `Subsymbol`.
   * @param other The `Subsymbol` instance to compare against.
   * @return `true` if all fields of both subsymbols match; `false` otherwise.
   */
  bool operator==(const Subsymbol& other) const {
    return subsym_idx == other.subsym_idx &&
           subsym_value == other.subsym_value &&
           prv_values[0] == other.prv_values[0] &&
           prv_values[1] == other.prv_values[1] &&
           lut_num_max_elems == other.lut_num_max_elems &&
           lut_entry_idx == other.lut_entry_idx;
  }

  /**
   * @brief Resets the subsymbol values to their default states.
   * @details Sets all member variables to zero, effectively clearing the
   * subsymbol state.
   */
  void Reset() {
    subsym_idx = 0;
    subsym_value = 0;
    prv_values[0] = 0;
    prv_values[1] = 0;
    lut_num_max_elems = 0;
    lut_entry_idx = 0;
  }

  /**
   * @brief Prints the details of the subsymbol for debugging purposes.
   * @details Outputs the current state of the subsymbol to the console.
   */
  void PrintDebug() const {
    const std::string debug_text =
        "Subsymbol Index: " + std::to_string(subsym_idx) +
        "\nSubsymbol Value: " + std::to_string(subsym_value) +
        "\nPrevious Values: [" + std::to_string(prv_values[0]) + ", " +
        std::to_string(prv_values[1]) + "]" +
        "\nLUT Max Elements: " + std::to_string(lut_num_max_elems) +
        "\nLUT Entry Index: " + std::to_string(lut_entry_idx);
    UTILS_LOG(util::Logger::Severity::INFO, debug_text);
  }
};

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_SUB_SYMBOL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
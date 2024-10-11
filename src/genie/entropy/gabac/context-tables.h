/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions to build context tables for GABAC entropy encoding and decoding.
 *
 * This file contains the declaration of functions for constructing context tables,
 * which are used in the GABAC (Genomic Adaptive Binary Arithmetic Coding) framework.
 * These context tables are essential for adaptive entropy encoding and decoding, providing
 * a structured mechanism for assigning probabilities to different contexts based on symbol
 * occurrences. The functions defined here support building context models with configurable
 * sizes for various compression and decompression tasks.
 *
 * @details The context tables are a key component in the CABAC (Context-based Adaptive Binary Arithmetic Coding)
 * process used by the GABAC library. The tables store the probability models for binary symbols, which are used
 * to estimate the likelihood of different bit patterns during the encoding and decoding phases. The build function
 * allows for dynamic creation of these tables based on the required number of contexts.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>                              // For standard integer types like uint64_t.
#include <vector>                               // For using std::vector in function parameters.
#include "genie/entropy/gabac/context-model.h"  // For the ContextModel class, which defines individual context states.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac::contexttables {

/**
 * @brief Constructs a context table for the GABAC framework.
 *
 * This function initializes a context table with a specified number of contexts.
 * Each context in the table is represented by a `ContextModel` object, which holds
 * the probability states for adaptive entropy coding. The size of the table is determined
 * by the `numContexts` parameter, allowing for flexible configurations depending on
 * the complexity of the data being processed.
 *
 * @param numContexts The number of contexts to include in the table. This value determines
 *                    the size of the returned context table.
 * @return A vector of `ContextModel` objects representing the context table for adaptive
 *         entropy coding.
 */
std::vector<ContextModel> buildContextTable(uint64_t numContexts);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac::contexttables

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_TABLES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

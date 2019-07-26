/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_CONTEXT_TABLES_H_
#define GABAC_CONTEXT_TABLES_H_

#include <vector>

#include "context-model.h"

namespace gabac {
namespace contexttables {

const int CONTEXT_SET_LENGTH = 32;

// These variables point to the location of the first context set for
// each type of binarization.
//
// Note: TEG/STEG are combinations of these "rudimentary" binarizations
//       (i.e., BI, TU and EG) and do not use dedicated context sets but
//       instead use a context set for each "sub"binarization.

const int OFFSET_TRUNCATED_UNARY_0 = 0;

const int OFFSET_EXPONENTIAL_GOLOMB_0 =
    OFFSET_TRUNCATED_UNARY_0 + (64 * CONTEXT_SET_LENGTH);

const int OFFSET_BINARY_0 =
    OFFSET_EXPONENTIAL_GOLOMB_0 + (16 * CONTEXT_SET_LENGTH);

// 64 rows, 32 columns, filled with 64
const std::vector<std::vector<unsigned char>> INIT_TRUNCATED_UNARY_CTX(
    64, std::vector<unsigned char>(32, 64));

// 16 rows, 32 columns, filled with 64
const std::vector<std::vector<unsigned char>> INIT_EXPONENTIAL_GOLOMB_CTX(
    16, std::vector<unsigned char>(32, 64));

// 16 rows, 32 columns, filled with 64
const std::vector<std::vector<unsigned char>> INIT_BINARY_CTX(
    16, std::vector<unsigned char>(32, 64));

std::vector<ContextModel> buildContextTable();

}  // namespace contexttables
}  // namespace gabac

#endif  // GABAC_CONTEXT_TABLES_H_

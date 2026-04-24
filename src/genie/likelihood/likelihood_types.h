/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_TYPES_H
#define GENIE_LIKELIHOOD_TYPES_H

#include <cstdint>
#include <vector>
#include <sstream>
#include "genie/backend/types.h"
#include "genie/core/constants.h"

namespace genie::likelihood {

// Unified backend types
using UInt32ArrDtype = genie::backend::UIntVecDtype;
using UInt32MatDtype = genie::backend::UIntMatDtype;
using MatShapeDtype = genie::backend::MatShapeDtype;

namespace detail {

struct LikelihoodEncodingBlock {
    uint32_t nrows;
    uint32_t ncols;
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;
    
    // Unified backend members
    UInt32MatDtype likelihood_mat;
    UInt32ArrDtype lut;
    UInt32MatDtype idx_mat;
    
    uint32_t nelems;
    core::DataType dtype_id = core::DataType::UINT32;

    LikelihoodEncodingBlock() : nrows(0), ncols(0), nelems(0) {}
};

} // namespace detail
} // namespace genie::likelihood

#endif // GENIE_LIKELIHOOD_TYPES_H
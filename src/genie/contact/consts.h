/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONSTS_H
#define GENIE_CONTACT_CONSTS_H

#include <cstdint>
#include <list>
#include <tuple>
//#include <optional>
//#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/contact_record/record.h"
// #include "contact_parameters.h"
//#include "subcontact_matrix_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

//using CountsDtype = uint32_t;
using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using UInt8VecDtype = xt::xtensor<uint8_t, 1, xt::layout_type::row_major>;
// using UInt8MatDtype = xt::xtensor<uint8_t , 2, xt::layout_type::row_major>;
// using Int8MatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
using UIntMatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;
using Int64VecDtype = xt::xtensor<int64_t, 1, xt::layout_type::row_major>;

using DoubleVecDtype = xt::xtensor<double_t, 1, xt::layout_type::row_major>;

//using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

//using ContactRecords = std::list<genie::core::record::ContactRecord>;

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONSTS_H

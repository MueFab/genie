/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_TYPES_H
#define GENIE_CONTACT_TYPES_H

#include <cstddef>
#include <cstdint>
#include <vector>

#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
#include <xtensor/xtensor.hpp>
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
#include <Eigen/Dense>
#endif

#include <genie/backend/types.h>

namespace genie::contact {

// Use consolidated backend types decoupled from global setting
#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
using BinVecDtype = ::genie::backend::genie_xt_impl::BinVecDtype;
using BinMatDtype = ::genie::backend::genie_xt_impl::BinMatDtype;
using UInt8VecDtype = ::genie::backend::genie_xt_impl::UInt8VecDtype;
using UIntVecDtype = ::genie::backend::genie_xt_impl::UIntVecDtype;
using UIntMatDtype = ::genie::backend::genie_xt_impl::UIntMatDtype;
using UInt64VecDtype = ::genie::backend::genie_xt_impl::UInt64VecDtype;
using MatShapeDtype = ::genie::backend::genie_xt_impl::MatShapeDtype;
using Int64VecDtype = xt::xtensor<int64_t, 1, xt::layout_type::row_major>;
using DoubleVecDtype = xt::xtensor<double, 1, xt::layout_type::row_major>;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
using BinVecDtype = ::genie::backend::genie_eigen_impl::BinVecDtype;
using BinMatDtype = ::genie::backend::genie_eigen_impl::BinMatDtype;
using UInt8VecDtype = ::genie::backend::genie_eigen_impl::UInt8VecDtype;
using UIntVecDtype = ::genie::backend::genie_eigen_impl::UIntVecDtype;
using UIntMatDtype = ::genie::backend::genie_eigen_impl::UIntMatDtype;
using UInt64VecDtype = ::genie::backend::genie_eigen_impl::UInt64VecDtype;
using MatShapeDtype = ::genie::backend::genie_eigen_impl::MatShapeDtype;
using Int64VecDtype = Eigen::Matrix<int64_t, Eigen::Dynamic, 1>;
using DoubleVecDtype = Eigen::Matrix<double, Eigen::Dynamic, 1>;
#else
using BinVecDtype = ::genie::backend::genie_std_impl::BinVecDtype;
using BinMatDtype = ::genie::backend::genie_std_impl::BinMatDtype;
using UInt8VecDtype = ::genie::backend::genie_std_impl::UInt8VecDtype;
using UIntVecDtype = ::genie::backend::genie_std_impl::UIntVecDtype;
using UIntMatDtype = ::genie::backend::genie_std_impl::UIntMatDtype;
using UInt64VecDtype = ::genie::backend::genie_std_impl::UInt64VecDtype;
using MatShapeDtype = ::genie::backend::genie_std_impl::MatShapeDtype;
using Int64VecDtype = std::vector<int64_t>;
using DoubleVecDtype = std::vector<double>;
#endif

enum class TransformID : uint8_t {
    ID_0 = 0,
    ID_1 = 1,
    ID_2 = 2,
    ID_3 = 3,
};

struct RunLengthEncodingData {
  bool firstVal;
  std::vector<uint32_t> rl_entries;
  uint32_t maxCount;
  TransformID transformID;
};

} // namespace genie::contact

#endif // GENIE_CONTACT_TYPES_H
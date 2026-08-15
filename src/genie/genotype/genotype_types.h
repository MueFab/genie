/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_GENOTYPE_GENOTYPE_TYPES_H_
#define SRC_GENIE_GENOTYPE_GENOTYPE_TYPES_H_

#include <cstdint>
#include <vector>

#if defined(GENIE_GENOTYPE_BACKEND_XTENSOR)
#include <xtensor/xtensor.hpp>
#elif defined(GENIE_GENOTYPE_BACKEND_EIGEN)
#include <Eigen/Dense>
#endif

#include <genie/backend/types.h>

namespace genie::genotype {

// Pure C++ types for records, parameter sets and blocks
#if defined(GENIE_GENOTYPE_BACKEND_XTENSOR)
using BinMatDtype = ::genie::backend::genie_xt_impl::BinMatDtype;
using UInt8MatDtype = ::genie::backend::genie_xt_impl::UInt8MatDtype;
using Int8MatDtype = ::genie::backend::genie_xt_impl::Int8MatDtype;
using UIntVecDtype = ::genie::backend::genie_xt_impl::UIntVecDtype;
using MatShapeDtype = ::genie::backend::genie_xt_impl::MatShapeDtype;
#elif defined(GENIE_GENOTYPE_BACKEND_EIGEN)
using BinMatDtype = ::genie::backend::genie_eigen_impl::BinMatDtype;
using UInt8MatDtype = ::genie::backend::genie_eigen_impl::UInt8MatDtype;
using Int8MatDtype = ::genie::backend::genie_eigen_impl::Int8MatDtype;
using UIntVecDtype = ::genie::backend::genie_eigen_impl::UIntVecDtype;
using MatShapeDtype = ::genie::backend::genie_eigen_impl::MatShapeDtype;
#else
using BinMatDtype = ::genie::backend::genie_std_impl::BinMatDtype;
using UInt8MatDtype = ::genie::backend::genie_std_impl::UInt8MatDtype;
using Int8MatDtype = ::genie::backend::genie_std_impl::Int8MatDtype;
using UIntVecDtype = ::genie::backend::genie_std_impl::UIntVecDtype;
using MatShapeDtype = ::genie::backend::genie_std_impl::MatShapeDtype;
#endif

}  // namespace genie::genotype

#endif  // SRC_GENIE_GENOTYPE_GENOTYPE_TYPES_H_

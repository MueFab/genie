#ifndef GENIE_BACKEND_BACKEND_EIGEN_H
#define GENIE_BACKEND_BACKEND_EIGEN_H

#include "backend.h"

#ifdef GENIE_HAS_EIGEN_BACKEND
#include <Eigen/Dense>
#endif

namespace genie::backend::genie_eigen_impl {

#ifdef GENIE_HAS_EIGEN_BACKEND
using BinVecDtype = ::genie::backend::genie_eigen_impl::BinVecDtype;
using BinMatDtype = ::genie::backend::genie_eigen_impl::BinMatDtype;
using UInt8VecDtype = ::genie::backend::genie_eigen_impl::UInt8VecDtype;
using UInt8MatDtype = ::genie::backend::genie_eigen_impl::UInt8MatDtype;
using UIntVecDtype = ::genie::backend::genie_eigen_impl::UIntVecDtype;
using UIntMatDtype = ::genie::backend::genie_eigen_impl::UIntMatDtype;
using UInt64VecDtype = ::genie::backend::genie_eigen_impl::UInt64VecDtype;
using Int8MatDtype = ::genie::backend::genie_eigen_impl::Int8MatDtype;
using MatShapeDtype = ::genie::backend::genie_eigen_impl::MatShapeDtype;
#endif

void sort_sparse_mat_inplace(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

void compute_mask(
    const UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
);

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
);

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
);

// Unified dispatcher overloads for Eigen
void serialize_mat(const UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
void serialize_arr(const UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, UIntMatDtype& mat);
void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, UIntVecDtype& arr);

} // namespace genie::backend::genie_eigen_impl

#endif // GENIE_BACKEND_BACKEND_EIGEN_H

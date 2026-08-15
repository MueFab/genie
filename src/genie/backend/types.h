#ifndef SRC_GENIE_BACKEND_TYPES_H_
#define SRC_GENIE_BACKEND_TYPES_H_

#include <cstdint>
#include <vector>
#include "genie/core/constants.h"

// Forward declarations or includes outside the namespace
// Synchronize and robustly define backend availability macros
#if defined(HAS_XTENSOR) || defined(GENIE_HAS_XTENSOR_BACKEND) || __has_include(<xtensor/xtensor.hpp>)
  #ifndef GENIE_HAS_XTENSOR_BACKEND
    #define GENIE_HAS_XTENSOR_BACKEND
  #endif
  #ifndef HAS_XTENSOR
    #define HAS_XTENSOR
  #endif
#endif

#if defined(HAS_EIGEN) || defined(GENIE_HAS_EIGEN_BACKEND) || __has_include(<Eigen/Dense>)
  #ifndef GENIE_HAS_EIGEN_BACKEND
    #define GENIE_HAS_EIGEN_BACKEND
  #endif
  #ifndef HAS_EIGEN
    #define HAS_EIGEN
  #endif
#endif

// Heavy backend includes must come BEFORE they are used in implementation namespaces
#ifdef GENIE_HAS_XTENSOR_BACKEND
#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xmath.hpp>
#endif

#ifdef GENIE_HAS_EIGEN_BACKEND
#include <Eigen/Dense>
#endif

namespace genie::backend {

namespace genie_std_impl {
    using BinVecDtype = std::vector<bool>;
    using BinMatDtype = std::vector<std::vector<bool>>;
    using UInt8VecDtype = std::vector<uint8_t>;
    using UInt8MatDtype = std::vector<std::vector<uint8_t>>;
    using UIntVecDtype = std::vector<uint32_t>;
    using UIntMatDtype = std::vector<std::vector<uint32_t>>;
    using UInt64VecDtype = std::vector<uint64_t>;
    using Int8MatDtype = std::vector<std::vector<int8_t>>;
    using MatShapeDtype = std::vector<size_t>;

    void sort_sparse_mat_inplace(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts);
    void bin_mat_to_bytes(const BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len);
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, BinMatDtype& bin_mat);
    void serialize_mat(const UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
    void serialize_arr(const UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, UIntMatDtype& mat);
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, UIntVecDtype& arr);
    void compute_mask(const UInt64VecDtype& ids, size_t nelems, BinVecDtype& mask);
}  // namespace genie_std_impl

#ifdef GENIE_HAS_XTENSOR_BACKEND
namespace genie_xt_impl {
    using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
    using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
    using UInt8VecDtype = xt::xtensor<uint8_t, 1, xt::layout_type::row_major>;
    using UInt8MatDtype = xt::xtensor<uint8_t, 2, xt::layout_type::row_major>;
    using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
    using UIntMatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
    using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;
    using Int8MatDtype = xt::xtensor<int8_t, 2, xt::layout_type::row_major>;
    using MatShapeDtype = std::vector<size_t>;

    void sort_sparse_mat_inplace(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts);
    void bin_mat_to_bytes(const BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len);
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, BinMatDtype& bin_mat);
    void serialize_mat(const UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
    void serialize_arr(const UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, UIntMatDtype& mat);
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, UIntVecDtype& arr);
    void compute_mask(const UInt64VecDtype& ids, size_t nelems, BinVecDtype& mask);
}  // namespace genie_xt_impl
#endif

#ifdef GENIE_HAS_EIGEN_BACKEND
namespace genie_eigen_impl {
    using BinVecDtype = Eigen::Matrix<bool, Eigen::Dynamic, 1>;
    using BinMatDtype = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using UInt8VecDtype = Eigen::Matrix<uint8_t, Eigen::Dynamic, 1>;
    using UInt8MatDtype = Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using UIntVecDtype = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;
    using UIntMatDtype = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using UInt64VecDtype = Eigen::Matrix<uint64_t, Eigen::Dynamic, 1>;
    using Int8MatDtype = Eigen::Matrix<int8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using MatShapeDtype = std::vector<Eigen::Index>;

    void sort_sparse_mat_inplace(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts);
    void bin_mat_to_bytes(const BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len);
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, BinMatDtype& bin_mat);
    void serialize_mat(const UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
    void serialize_arr(const UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, UIntMatDtype& mat);
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, UIntVecDtype& arr);
    void compute_mask(const UInt64VecDtype& ids, size_t nelems, BinVecDtype& mask);
}  // namespace genie_eigen_impl
#endif

// Default global aliases based on GENIE_BACKEND setting
#if defined(GENIE_BACKEND_XTENSOR) && defined(GENIE_HAS_XTENSOR_BACKEND)
    using BinVecDtype = genie_xt_impl::BinVecDtype;
    using BinMatDtype = genie_xt_impl::BinMatDtype;
    using UInt8VecDtype = genie_xt_impl::UInt8VecDtype;
    using UInt8MatDtype = genie_xt_impl::UInt8MatDtype;
    using UIntVecDtype = genie_xt_impl::UIntVecDtype;
    using UIntMatDtype = genie_xt_impl::UIntMatDtype;
    using UInt64VecDtype = genie_xt_impl::UInt64VecDtype;
    using Int8MatDtype = genie_xt_impl::Int8MatDtype;
#elif defined(GENIE_BACKEND_EIGEN) && defined(GENIE_HAS_EIGEN_BACKEND)
    using BinVecDtype = genie_eigen_impl::BinVecDtype;
    using BinMatDtype = genie_eigen_impl::BinMatDtype;
    using UInt8VecDtype = genie_eigen_impl::UInt8VecDtype;
    using UInt8MatDtype = genie_eigen_impl::UInt8MatDtype;
    using UIntVecDtype = genie_eigen_impl::UIntVecDtype;
    using UIntMatDtype = genie_eigen_impl::UIntMatDtype;
    using UInt64VecDtype = genie_eigen_impl::UInt64VecDtype;
    using Int8MatDtype = genie_eigen_impl::Int8MatDtype;
#else
    using BinVecDtype = genie_std_impl::BinVecDtype;
    using BinMatDtype = genie_std_impl::BinMatDtype;
    using UInt8VecDtype = genie_std_impl::UInt8VecDtype;
    using UInt8MatDtype = genie_std_impl::UInt8MatDtype;
    using UIntVecDtype = genie_std_impl::UIntVecDtype;
    using UIntMatDtype = genie_std_impl::UIntMatDtype;
    using UInt64VecDtype = genie_std_impl::UInt64VecDtype;
    using Int8MatDtype = genie_std_impl::Int8MatDtype;
#endif

using MatShapeDtype = std::vector<size_t>;

}  // namespace genie::backend

#endif  // SRC_GENIE_BACKEND_TYPES_H_

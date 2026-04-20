#include "backend.h"
#include "backend_std.h"

#ifdef GENIE_HAS_XTENSOR_BACKEND
#include "backend_xtensor.h"
#endif

#ifdef GENIE_HAS_EIGEN_BACKEND
#include "backend_eigen.h"
#endif

namespace genie::backend {

// --- Explicit Namespace Implementations ---
// These provide stable symbols for non-template consumers if needed.

namespace std_lib {
    void sort_sparse_mat_inplace(::genie::backend::genie_std_impl::UInt64VecDtype& row_ids, ::genie::backend::genie_std_impl::UInt64VecDtype& col_ids, ::genie::backend::genie_std_impl::UIntVecDtype& counts) {
        ::genie::backend::genie_std_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
    }
    void bin_mat_to_bytes(const ::genie::backend::genie_std_impl::BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len) {
        ::genie::backend::genie_std_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
    }
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, ::genie::backend::genie_std_impl::BinMatDtype& bin_mat) {
        ::genie::backend::genie_std_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
    }
    void serialize_mat(const ::genie::backend::genie_std_impl::UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload) {
        ::genie::backend::genie_std_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
    }
    void serialize_arr(const ::genie::backend::genie_std_impl::UIntVecDtype& arr, uint32_t nelems, std::ostream& payload) {
        ::genie::backend::genie_std_impl::serialize_arr(arr, nelems, payload);
    }
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, ::genie::backend::genie_std_impl::UIntMatDtype& mat) {
        ::genie::backend::genie_std_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
    }
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, ::genie::backend::genie_std_impl::UIntVecDtype& arr) {
        ::genie::backend::genie_std_impl::deserialize_arr(payload_bytes, nelems, arr);
    }
    void compute_mask(const ::genie::backend::genie_std_impl::UInt64VecDtype& ids, size_t nelems, ::genie::backend::genie_std_impl::BinVecDtype& mask) {
        ::genie::backend::genie_std_impl::compute_mask(ids, nelems, mask);
    }
}

#ifdef GENIE_HAS_XTENSOR_BACKEND
namespace xtensor {
    void sort_sparse_mat_inplace(::genie::backend::genie_xt_impl::UInt64VecDtype& row_ids, ::genie::backend::genie_xt_impl::UInt64VecDtype& col_ids, ::genie::backend::genie_xt_impl::UIntVecDtype& counts) {
        ::genie::backend::genie_xt_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
    }
    void bin_mat_to_bytes(const ::genie::backend::genie_xt_impl::BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len) {
        ::genie::backend::genie_xt_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
    }
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, ::genie::backend::genie_xt_impl::BinMatDtype& bin_mat) {
        ::genie::backend::genie_xt_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
    }
    void serialize_mat(const ::genie::backend::genie_xt_impl::UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload) {
        ::genie::backend::genie_xt_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
    }
    void serialize_arr(const ::genie::backend::genie_xt_impl::UIntVecDtype& arr, uint32_t nelems, std::ostream& payload) {
        ::genie::backend::genie_xt_impl::serialize_arr(arr, nelems, payload);
    }
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, ::genie::backend::genie_xt_impl::UIntMatDtype& mat) {
        ::genie::backend::genie_xt_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
    }
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, ::genie::backend::genie_xt_impl::UIntVecDtype& arr) {
        ::genie::backend::genie_xt_impl::deserialize_arr(payload_bytes, nelems, arr);
    }
    void compute_mask(const ::genie::backend::genie_xt_impl::UInt64VecDtype& ids, size_t nelems, ::genie::backend::genie_xt_impl::BinVecDtype& mask) {
        ::genie::backend::genie_xt_impl::compute_mask(ids, nelems, mask);
    }
}
#endif

#ifdef GENIE_HAS_EIGEN_BACKEND
namespace eigen {
    void sort_sparse_mat_inplace(::genie::backend::genie_eigen_impl::UInt64VecDtype& row_ids, ::genie::backend::genie_eigen_impl::UInt64VecDtype& col_ids, ::genie::backend::genie_eigen_impl::UIntVecDtype& counts) {
        ::genie::backend::genie_eigen_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
    }
    void bin_mat_to_bytes(const ::genie::backend::genie_eigen_impl::BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len) {
        ::genie::backend::genie_eigen_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
    }
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, ::genie::backend::genie_eigen_impl::BinMatDtype& bin_mat) {
        ::genie::backend::genie_eigen_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
    }
    void serialize_mat(const ::genie::backend::genie_eigen_impl::UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload) {
        ::genie::backend::genie_eigen_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
    }
    void serialize_arr(const ::genie::backend::genie_eigen_impl::UIntVecDtype& arr, uint32_t nelems, std::ostream& payload) {
        ::genie::backend::genie_eigen_impl::serialize_arr(arr, nelems, payload);
    }
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, ::genie::backend::genie_eigen_impl::UIntMatDtype& mat) {
        ::genie::backend::genie_eigen_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
    }
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, ::genie::backend::genie_eigen_impl::UIntVecDtype& arr) {
        ::genie::backend::genie_eigen_impl::deserialize_arr(payload_bytes, nelems, arr);
    }
    void compute_mask(const ::genie::backend::genie_eigen_impl::UInt64VecDtype& ids, size_t nelems, ::genie::backend::genie_eigen_impl::BinVecDtype& mask) {
        ::genie::backend::genie_eigen_impl::compute_mask(ids, nelems, mask);
    }
}
#endif

} // namespace genie::backend

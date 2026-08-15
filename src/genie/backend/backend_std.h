#ifndef SRC_GENIE_BACKEND_BACKEND_STD_H_
#define SRC_GENIE_BACKEND_BACKEND_STD_H_

#include "genie/backend/backend.h"
#include <vector>

namespace genie::backend::genie_std_impl {

// Aliases already defined in types.h within this namespace

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

// Unified dispatcher overloads for STD
void serialize_mat(const UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
void serialize_arr(const UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, UIntMatDtype& mat);
void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, UIntVecDtype& arr);

}  // namespace genie::backend::genie_std_impl

#endif  // SRC_GENIE_BACKEND_BACKEND_STD_H_

#include <xtensor/xindex_view.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <genie/util/runtime_exception.h>
#include <genie/util/bit_writer.h>
#include <genie/util/bit_reader.h>
#include <numeric>
#include <sstream>
#include "backend.h"
#include "backend_xtensor.h"

namespace genie::backend::genie_xt_impl {

void sort_sparse_mat_inplace(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
) {
    size_t num_entries = counts.size();
    if (num_entries == 0) return;

    std::vector<size_t> sort_indices(num_entries);
    std::iota(sort_indices.begin(), sort_indices.end(), 0);

    std::stable_sort(
        sort_indices.begin(),
        sort_indices.end(),
        [&](size_t i1, size_t i2) {
            if (row_ids(i1) != row_ids(i2)) {
                return row_ids(i1) < row_ids(i2);
            }
            return col_ids(i1) < col_ids(i2);
        }
    );

    UInt64VecDtype sorted_row_ids = xt::empty_like(row_ids);
    UInt64VecDtype sorted_col_ids = xt::empty_like(col_ids);
    UIntVecDtype sorted_counts = xt::empty_like(counts);

    for (size_t idx_k = 0u; idx_k < num_entries; idx_k++) {
        sorted_row_ids(idx_k) = row_ids(sort_indices[idx_k]);
        sorted_col_ids(idx_k) = col_ids(sort_indices[idx_k]);
        sorted_counts(idx_k) = counts(sort_indices[idx_k]);
    }

    row_ids = std::move(sorted_row_ids);
    col_ids = std::move(sorted_col_ids);
    counts = std::move(sorted_counts);
}

void compute_mask(
    const UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
) {
    mask = xt::zeros<bool>({nelems});
    for (size_t idx_i = 0; idx_i < ids.shape(0); ++idx_i) {
        if (ids(idx_i) < nelems) {
            mask(ids(idx_i)) = true;
        }
    }
}

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = static_cast<size_t>(bin_mat.shape(0));
    auto ncols = static_cast<size_t>(bin_mat.shape(1));

    if (nrows == 0) {
        *payload = nullptr;
        payload_len = 0;
        return;
    }

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    payload_len = bpl * nrows;
    *payload = (uint8_t*)calloc(payload_len, sizeof(uint8_t));

    for (size_t idx_i = 0u; idx_i < nrows; idx_i++) {
        size_t row_offset = idx_i * bpl;
        for (size_t idx_j = 0u; idx_j < ncols; idx_j++) {
            auto byte_offset = row_offset + (idx_j >> 3u);
            uint8_t shift = (7u - (idx_j & 7u));
            if (bin_mat(idx_i, idx_j)) {
                (*payload)[byte_offset] |= (1u << shift);
            }
        }
    }
}

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len in XTensor bin_mat_from_bytes!");

    bin_mat = xt::zeros<bool>({nrows, ncols});

    for (size_t idx_i = 0u; idx_i < nrows; idx_i++) {
        size_t row_offset = idx_i * bpl;
        for (size_t idx_j = 0u; idx_j < ncols; idx_j++) {
            auto byte_offset = row_offset + (idx_j >> 3u);
            uint8_t shift = (7u - (idx_j & 7u));
            bin_mat(idx_i, idx_j) = (payload[byte_offset] >> shift) & 1u;
        }
    }
}

void serialize_mat(
    const UIntMatDtype& mat,
    uint32_t dtype_id,
    uint32_t nrows,
    uint32_t ncols,
    std::ostream& payload
) {
    util::BitWriter writer(&payload);
    for (size_t idx_i = 0; idx_i < nrows; idx_i++) {
        for (size_t idx_j = 0; idx_j < ncols; idx_j++) {
            if (dtype_id == 0) // UINT8
                writer.WriteBypassBE<uint8_t>(static_cast<uint8_t>(mat(idx_i, idx_j)));
            else if (dtype_id == 1) // UINT16
                writer.WriteBypassBE<uint16_t>(static_cast<uint16_t>(mat(idx_i, idx_j)));
            else
                writer.WriteBypassBE<uint32_t>(mat(idx_i, idx_j));
        }
    }
}

void serialize_arr(
    const UIntVecDtype& arr,
    uint32_t nelems,
    std::ostream& payload
) {
    util::BitWriter writer(&payload);
    for (size_t idx_i = 0; idx_i < nelems; idx_i++)
        writer.WriteBypassBE<uint32_t>(arr(idx_i));
}

void deserialize_mat(
    const std::vector<uint8_t>& payload_bytes,
    uint32_t dtype_id,
    uint32_t nrows,
    uint32_t ncols,
    UIntMatDtype& mat
) {
    std::string s(payload_bytes.begin(), payload_bytes.end());
    std::stringstream stream(s);
    util::BitReader reader(stream);
    mat = xt::empty<uint32_t>({nrows, ncols});
    for (size_t idx_i = 0; idx_i < nrows; idx_i++) {
        for (size_t idx_j = 0; idx_j < ncols; idx_j++) {
            if (dtype_id == 0) // UINT8
                mat(idx_i, idx_j) = reader.ReadAlignedInt<uint8_t>();
            else if (dtype_id == 1) // UINT16
                mat(idx_i, idx_j) = reader.ReadAlignedInt<uint16_t>();
            else
                mat(idx_i, idx_j) = reader.ReadAlignedInt<uint32_t>();
        }
    }
}

void deserialize_arr(
    const std::vector<uint8_t>& payload_bytes,
    uint32_t nelems,
    UIntVecDtype& arr
) {
    std::string s(payload_bytes.begin(), payload_bytes.end());
    std::stringstream stream(s);
    util::BitReader reader(stream);
    arr = xt::empty<uint32_t>({nelems});
    for (size_t idx_i = 0; idx_i < nelems; ++idx_i)
        arr(idx_i) = reader.ReadAlignedInt<uint32_t>();
}

} // namespace genie::backend::genie_xt_impl

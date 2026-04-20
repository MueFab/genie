#include "backend_std.h"
#include <genie/util/runtime_exception.h>
#include <genie/util/bit_writer.h>
#include <genie/util/bit_reader.h>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace genie::backend::genie_std_impl {

void sort_sparse_mat_inplace(
    std::vector<uint64_t>& tile_row_ids,
    std::vector<uint64_t>& tile_col_ids,
    std::vector<uint32_t>& tile_counts
) {
    size_t num_entries = tile_counts.size();
    if (num_entries == 0) return;

    std::vector<size_t> sort_indices(num_entries);
    std::iota(sort_indices.begin(), sort_indices.end(), 0);

    std::stable_sort(
        sort_indices.begin(),
        sort_indices.end(),
        [&](size_t i1, size_t i2) {
            if (tile_row_ids[i1] != tile_row_ids[i2]) {
                return tile_row_ids[i1] < tile_row_ids[i2];
            }
            return tile_col_ids[i1] < tile_col_ids[i2];
        }
    );

    std::vector<uint64_t> sorted_row_ids(num_entries);
    std::vector<uint64_t> sorted_col_ids(num_entries);
    std::vector<uint32_t> sorted_counts(num_entries);

    for (size_t idx_k = 0u; idx_k < num_entries; idx_k++) {
        sorted_row_ids[idx_k] = tile_row_ids[sort_indices[idx_k]];
        sorted_col_ids[idx_k] = tile_col_ids[sort_indices[idx_k]];
        sorted_counts[idx_k] = tile_counts[sort_indices[idx_k]];
    }

    tile_row_ids = std::move(sorted_row_ids);
    tile_col_ids = std::move(sorted_col_ids);
    tile_counts = std::move(sorted_counts);
}

void compute_mask(
    const std::vector<uint64_t>& ids,
    size_t nelems,
    std::vector<bool>& mask
) {
    mask.assign(nelems, false);
    std::vector<uint64_t> ids_copy = ids;
    std::sort(ids_copy.begin(), ids_copy.end());
    ids_copy.erase(std::unique(ids_copy.begin(), ids_copy.end()), ids_copy.end());

    for (auto id : ids_copy) {
        if (id < nelems) {
            mask[id] = true;
        }
    }
}

void bin_mat_to_bytes(
    const std::vector<std::vector<bool>>& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = bin_mat.size();
    if (nrows == 0) {
        *payload = nullptr;
        payload_len = 0;
        return;
    }
    auto ncols = bin_mat[0].size();

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    payload_len = bpl * nrows;
    *payload = (uint8_t*)calloc(payload_len, sizeof(uint8_t));

    for (size_t idx_i = 0u; idx_i < nrows; idx_i++) {
        size_t row_offset = idx_i * bpl;
        for (size_t idx_j = 0u; idx_j < ncols; idx_j++) {
            auto byte_offset = row_offset + (idx_j >> 3u);
            uint8_t shift = (7u - (idx_j & 7u));
            if (bin_mat[idx_i][idx_j]) {
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
    std::vector<std::vector<bool>>& bin_mat
) {
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len in bin_mat_from_bytes!");

    bin_mat.assign(nrows, std::vector<bool>(ncols, false));

    for (size_t idx_i = 0u; idx_i < nrows; idx_i++) {
        size_t row_offset = idx_i * bpl;
        for (size_t idx_j = 0u; idx_j < ncols; idx_j++) {
            auto byte_offset = row_offset + (idx_j >> 3u);
            uint8_t shift = (7u - (idx_j & 7u));
            bin_mat[idx_i][idx_j] = (payload[byte_offset] >> shift) & 1u;
        }
    }
}

void serialize_mat(
    const std::vector<std::vector<uint32_t>>& mat,
    uint32_t dtype_id,
    uint32_t nrows,
    uint32_t ncols,
    std::ostream& payload
) {
    util::BitWriter writer(&payload);
    for (size_t idx_i = 0; idx_i < nrows; idx_i++) {
        for (size_t idx_j = 0; idx_j < ncols; idx_j++) {
            if (dtype_id == 0) // UINT8
                writer.WriteBypassBE<uint8_t>(static_cast<uint8_t>(mat[idx_i][idx_j]));
            else if (dtype_id == 1) // UINT16
                writer.WriteBypassBE<uint16_t>(static_cast<uint16_t>(mat[idx_i][idx_j]));
            else
                writer.WriteBypassBE<uint32_t>(mat[idx_i][idx_j]);
        }
    }
}

void serialize_arr(
    const std::vector<uint32_t>& arr,
    uint32_t nelems,
    std::ostream& payload
) {
    util::BitWriter writer(&payload);
    for (size_t idx_i = 0; idx_i < nelems; idx_i++)
        writer.WriteBypassBE<uint32_t>(arr[idx_i]);
}

void deserialize_mat(
    const std::vector<uint8_t>& payload_bytes,
    uint32_t dtype_id,
    uint32_t nrows,
    uint32_t ncols,
    std::vector<std::vector<uint32_t>>& mat
) {
    std::string s(payload_bytes.begin(), payload_bytes.end());
    std::stringstream stream(s);
    util::BitReader reader(stream);
    mat.assign(nrows, std::vector<uint32_t>(ncols, 0));
    for (size_t idx_i = 0; idx_i < nrows; idx_i++) {
        for (size_t idx_j = 0; idx_j < ncols; idx_j++) {
            if (dtype_id == 0) // UINT8
                mat[idx_i][idx_j] = reader.ReadAlignedInt<uint8_t>();
            else if (dtype_id == 1) // UINT16
                mat[idx_i][idx_j] = reader.ReadAlignedInt<uint16_t>();
            else
                mat[idx_i][idx_j] = reader.ReadAlignedInt<uint32_t>();
        }
    }
}

void deserialize_arr(
    const std::vector<uint8_t>& payload_bytes,
    uint32_t nelems,
    std::vector<uint32_t>& arr
) {
    std::string s(payload_bytes.begin(), payload_bytes.end());
    std::stringstream stream(s);
    util::BitReader reader(stream);
    arr.resize(nelems);
    for (size_t idx_i = 0; idx_i < nelems; ++idx_i)
        arr[idx_i] = reader.ReadAlignedInt<uint32_t>();
}

} // namespace genie::backend::genie_std_impl

#include "contact_coder_std.h"
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/contact_record/record.h>
#include <genie/util/runtime_exception.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <map>
#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_mask_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact::detail::std_lib {

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const std::vector<bool>& scm_mask
) {
    if (scm_mask.empty()) {
        rleData.maxCount = 0;
        rleData.transformID = TransformID::ID_0;
        return;
    }

    rleData.firstVal = scm_mask[0];
    rleData.rl_entries.clear();

    uint32_t current_count = 0;
    bool current_val = scm_mask[0];

    for (bool val : scm_mask) {
        if (val == current_val) {
            current_count++;
        } else {
            rleData.rl_entries.push_back(current_count);
            current_val = val;
            current_count = 1;
        }
    }
    rleData.rl_entries.push_back(current_count);

    if (!rleData.rl_entries.empty()) {
        rleData.maxCount = *std::max_element(rleData.rl_entries.begin(), rleData.rl_entries.end());
    } else {
        rleData.maxCount = 0;
    }

    if (rleData.maxCount <= 255) {
        rleData.transformID = TransformID::ID_1;
    } else if (rleData.maxCount <= 65535) {
        rleData.transformID = TransformID::ID_2;
    } else {
        rleData.transformID = TransformID::ID_3;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_mask(
    std::vector<uint64_t>& ids,
    size_t nelems,
    std::vector<bool>& mask
){
    mask.assign(nelems, false);
    if (ids.empty()) {
        return;
    }
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    for (auto id : ids){
        if (id < nelems) {
            mask[id] = true;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    size_t nrows,
    size_t ncols,
    const bool is_intra_scm,
    std::vector<bool>& row_mask,
    std::vector<bool>& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        std::vector<bool> mask;
        std::vector<uint64_t> combined_ids;
        combined_ids.reserve(row_ids.size() + col_ids.size());
        combined_ids.insert(combined_ids.end(), row_ids.begin(), row_ids.end());
        combined_ids.insert(combined_ids.end(), col_ids.begin(), col_ids.end());

        compute_mask(combined_ids, nrows, mask);

        row_mask = mask;
        col_mask = mask;
    } else {
        compute_mask(row_ids, nrows, row_mask);
        compute_mask(col_ids, ncols, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    std::vector<bool>& row_mask,
    std::vector<bool>& col_mask
){
    auto row_nentries = cm_param.GetNumBinEntries(scm_param.GetChr1ID());
    auto col_nentries = cm_param.GetNumBinEntries(scm_param.GetChr2ID());

    if (scm_param.GetRowMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        row_mask.assign(row_nentries, true);
    }

    if (scm_param.IsIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
        col_mask.assign(col_nentries, true);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    std::vector<bool>& mask
) {
    auto transform_ID = mask_payload.GetTransformID();
    if (transform_ID == TransformID::ID_0){
        mask = mask_payload.GetMaskArray();
        UTILS_DIE_IF(
            num_entries != mask.size(),
            "num_entries and the size of mask_array_ differ!"
        );
    } else {
        mask.assign(num_entries, false);

        bool first_val = mask_payload.GetFirstVal();
        const auto& rl_entries = mask_payload.GetRlEntries();

        size_t start_idx = 0;
        for (const auto& rl_entry : rl_entries){
            size_t end_idx = start_idx + rl_entry;
            std::fill(mask.begin() + start_idx, mask.begin() + std::min(end_idx, num_entries), first_val);
            start_idx = end_idx;
            first_val = !first_val;
        }
        UTILS_DIE_IF(
            start_idx > num_entries,
            "start_idx value must be smaller than num_entries!"
        );
        if (start_idx < num_entries) {
            std::fill(mask.begin() + start_idx, mask.end(), first_val);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    bool is_intra_tile,
    const std::vector<bool>& row_mask,
    const std::vector<bool>& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.size();
        const auto& mask = row_mask;
        auto mapping_len = mask.size();
        std::vector<uint64_t> mapping(mapping_len);
        uint64_t new_id = 0u;
        for (size_t i = 0u; i<mapping_len; i++){
            mapping[i] = new_id;
            if (mask[i]){
                new_id++;
            }
        }

        for (size_t i = 0u; i<num_entries; i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }
    } else {
        auto row_mapping_len = row_mask.size();
        std::vector<uint64_t> row_mapping(row_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t i = 0u; i<row_mapping_len; i++){
                row_mapping[i] = new_id;
                if (row_mask[i]){
                    new_id++;
                }
            }
        }
        auto num_entries = row_ids.size();
        for (size_t i = 0u; i<num_entries; i++){
            row_ids[i] = row_mapping[row_ids[i]];
        }

        auto col_mapping_len = col_mask.size();
        std::vector<uint64_t> col_mapping(col_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t i = 0u; i<col_mapping_len; i++){
                col_mapping[i] = new_id;
                if (col_mask[i]){
                    new_id++;
                }
            }
        }
        num_entries = col_ids.size();
        for (size_t i = 0u; i<num_entries; i++){
            col_ids[i] = col_mapping[col_ids[i]];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void insert_unaligned(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    bool is_intra_tile,
    const std::vector<bool>& row_mask,
    const std::vector<bool>& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.size();
        auto& mask = row_mask;
        std::vector<uint64_t> mapping;
        for(size_t i=0; i<mask.size(); ++i) {
            if(mask[i]) {
                mapping.push_back(i);
            }
        }

        for (size_t i = 0u; i<num_entries; i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }

    } else {
        {
            auto num_entries = row_ids.size();
            std::vector<uint64_t> mapping;
            for(size_t i=0; i<row_mask.size(); ++i) {
                if(row_mask[i]) {
                    mapping.push_back(i);
                }
            }

            for (size_t i = 0u; i<num_entries; i++){
                row_ids[i] = mapping[row_ids[i]];
            }
        }

        {
            auto num_entries = col_ids.size();
            std::vector<uint64_t> mapping;
            for(size_t i=0; i<col_mask.size(); ++i) {
                if(col_mask[i]) {
                    mapping.push_back(i);
                }
            }
            for (size_t i = 0u; i<num_entries; i++){
                col_ids[i] = mapping[col_ids[i]];
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    const std::vector<uint64_t>& row_ids,
    const std::vector<uint64_t>& col_ids,
    const std::vector<uint32_t>& counts,
    size_t nrows,
    size_t ncols,
    std::vector<std::vector<uint32_t>>& mat
){
    if (row_ids.size() > 0) {
        UTILS_DIE_IF(*std::max_element(row_ids.begin(), row_ids.end()) >= nrows, "Invalid nrows or row_ids!");
    }
    if (col_ids.size() > 0) {
        UTILS_DIE_IF(*std::max_element(col_ids.begin(), col_ids.end()) >= ncols, "Invalid ncols or col_ids!");
    }
    if (counts.size() > 0) {
        UTILS_DIE_IF(std::any_of(counts.begin(), counts.end(), [](uint32_t c){ return c == 0; }), "Count with value 0 is found!");
    }
    mat.assign(nrows, std::vector<uint32_t>(ncols, 0));

    for (size_t i = 0u; i< counts.size(); i++){
        mat[row_ids[i]][col_ids[i]] = counts[i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    const std::vector<std::vector<uint32_t>>& mat,
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    std::vector<uint32_t>& counts
){
    row_ids.clear();
    col_ids.clear();
    counts.clear();
    for (size_t i = 0; i < mat.size(); ++i) {
        for (size_t j = 0; j < mat[i].size(); ++j) {
            if (mat[i][j] > 0) {
                row_ids.push_back(i);
                col_ids.push_back(j);
                counts.push_back(mat[i][j]);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_sparse_mat_inplace(
    std::vector<uint64_t>& tile_row_ids,
    std::vector<uint64_t>& tile_col_ids,
    std::vector<uint32_t>& tile_counts
){
    size_t num_entries = tile_counts.size();

    assert(tile_row_ids.size() == num_entries);
    assert(tile_col_ids.size() == num_entries);

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

    bool already_sorted = true;
    for(size_t i = 0; i < num_entries; ++i) {
        if(sort_indices[i] != i) {
            already_sorted = false;
            break;
        }
    }
    if (already_sorted) {
        return;
    }

    std::vector<uint64_t> sorted_row_ids(num_entries);
    std::vector<uint64_t> sorted_col_ids(num_entries);
    std::vector<uint32_t> sorted_counts(num_entries);

    for (size_t k = 0u; k < num_entries; k++){
        sorted_row_ids[k] = tile_row_ids[sort_indices[k]];
        sorted_col_ids[k] = tile_col_ids[sort_indices[k]];
        sorted_counts[k] = tile_counts[sort_indices[k]];
    }

    tile_row_ids = sorted_row_ids;
    tile_col_ids = sorted_col_ids;
    tile_counts = sorted_counts;
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    std::vector<std::vector<uint32_t>>& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) {
        return;
    }

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    std::vector<std::vector<uint32_t>> trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        auto source_ncols = ncols;
        auto target_nrows = source_ncols;

        trans_mat.assign(target_nrows, std::vector<uint32_t>(target_nrows, 0));

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < source_ncols; ++k_diag) {
            for (int64_t i_src = 0; i_src < (source_ncols - k_diag); ++i_src) {
                int64_t j_src = i_src + k_diag;

                if (o >= nrows * ncols) {
                    goto end_mode_0_inverse_transform;
                }

                int64_t i_mat = o / ncols;
                int64_t j_mat = o % ncols;

                if (i_mat < nrows && j_mat < ncols) {
                    trans_mat[i_src][j_src] = mat[i_mat][j_mat];
                }
                o++;
            }
        }
        end_mode_0_inverse_transform:;
        mat = trans_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    std::vector<std::vector<uint32_t>>& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE){
        return;
    }

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    std::vector<std::vector<uint32_t>> trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(
            nrows != ncols,
            "Matrix must be a square!"
        );

        auto new_nrows = nrows / 2 + 1;
        trans_mat.assign(new_nrows, std::vector<uint32_t>(ncols, 0));

        size_t o = 0u;
        for (size_t k_diag = 0; k_diag < nrows; ++k_diag) {
            for (size_t i = 0; i < (nrows - k_diag); ++i) {
                size_t j = i + k_diag;

                auto v = mat[i][j];
                size_t target_i = o / ncols;
                size_t target_j = o % ncols;

                if (target_i < new_nrows && target_j < ncols) {
                    trans_mat[target_i][target_j] = v;
                }
                o++;
            }
        }
        mat = trans_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    const std::vector<std::vector<bool>>& bin_mat,
    std::vector<std::vector<uint32_t>>& mat
){
    size_t bin_mat_nrows = bin_mat.size();
    if (bin_mat_nrows == 0) return;
    size_t bin_mat_ncols = bin_mat[0].size();

    UTILS_DIE_IF(bin_mat_nrows == 0, "Invalid bin_mat_nrows!");
    UTILS_DIE_IF(bin_mat_ncols == 0, "Invalid bin_mat_ncols!");

    size_t mat_ncols = bin_mat_ncols - 1;

    size_t mat_nrows = 0;
    for(const auto& row : bin_mat) {
        if(row[0]) {
            mat_nrows++;
        }
    }

    UTILS_DIE_IF(mat_nrows == 0, "Invalid mat_nrows after transformation!");
    UTILS_DIE_IF(mat_ncols == 0, "Invalid mat_ncols after transformation!");

    mat.assign(mat_nrows, std::vector<uint32_t>(mat_ncols, 0));

    size_t target_i = 0;
    uint8_t bit_pos = 0;

    for (size_t i = 0; i < bin_mat_nrows; ++i){
        for (size_t j = 1; j < bin_mat_ncols; ++j) {
            if (bin_mat[i][j]) {
                mat[target_i][j-1] |= (1u << bit_pos);
            }
        }

        if (bin_mat[i][0]){
            target_i++;
            bit_pos = 0;
        } else {
            bit_pos++;
        }
    }

    UTILS_DIE_IF(target_i != mat_nrows, "Not all of the mat rows are processed!");
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_row_bin(
    const std::vector<std::vector<uint32_t>>& mat,
    std::vector<std::vector<bool>>& bin_mat
) {
    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    std::vector<uint8_t> nbits_per_row(nrows);
    for(size_t i=0; i<nrows; ++i) {
        uint32_t max_val = 0;
        if (!mat[i].empty()) {
            max_val = *std::max_element(mat[i].begin(), mat[i].end());
        }
        nbits_per_row[i] = (max_val == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(max_val + 1u)));
        if (nbits_per_row[i] == 0) nbits_per_row[i] = 1;
    }


    uint64_t bin_mat_nrows = std::accumulate(nbits_per_row.begin(), nbits_per_row.end(), 0);
    uint64_t bin_mat_ncols = ncols + 1;

    bin_mat.assign(bin_mat_nrows, std::vector<bool>(bin_mat_ncols, false));

    size_t current_bin_mat_row = 0;
    for (size_t i = 0; i < nrows; ++i) {
        auto bitlength = nbits_per_row[i];
        for (size_t i_bit = 0; i_bit < bitlength; ++i_bit) {
            for(size_t j=0; j<ncols; ++j) {
                if(mat[i][j] & (1u << i_bit)) {
                    bin_mat[current_bin_mat_row][j+1] = true;
                }
            }
            current_bin_mat_row++;
        }
        if (current_bin_mat_row > 0) {
            bin_mat[current_bin_mat_row - 1][0] = true;
        }
    }
}


// ---------------------------------------------------------------------------------------------------------------------

void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
){
    start_idx = tile_idx * tile_size;
    end_idx = std::min(start_idx + tile_size, num_entries);
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = bin_mat.size();
    if (nrows == 0) return;
    auto ncols = bin_mat[0].size();

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // Ceil div operation
    payload_len = bpl * nrows;
    *payload = (unsigned char*) calloc (payload_len, sizeof(unsigned char));

    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = (7u - (j & 7u));
            
            auto val = static_cast<uint8_t>(bin_mat[i][j]);
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // bytes per line with ceil operation
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len / nrows / ncols!");

    bin_mat.assign(nrows, std::vector<bool>(ncols, false));

    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = (7u - (j & 7u));
            bin_mat[i][j] = (*(payload + byte_offset) >> shift) & 1u;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
){
    uint8_t* raw_data;
    size_t raw_data_len;
    uint8_t* compressed_data;
    size_t compressed_data_len;

    unsigned long tile_nrows;
    unsigned long tile_ncols;

    if (codec_ID == core::AlgoID::JBIG){
        compressed_data_len = tile_payload.GetPayloadSize();
        auto& payload = tile_payload.GetPayload(); // This is std::vector<uint8_t>

        compressed_data = (uint8_t*)malloc(compressed_data_len * sizeof(uint8_t));
        memcpy(compressed_data, payload.data(), compressed_data_len);

        mpegg_jbig_decompress_default(
            &raw_data,
            &raw_data_len,
            compressed_data,
            compressed_data_len,
            &tile_nrows,
            &tile_ncols
        );

        free(compressed_data);

        bin_mat_from_bytes(
            raw_data,
            raw_data_len,
            static_cast<size_t>(tile_nrows),
            static_cast<size_t>(tile_ncols),
            bin_mat
        );

        free(raw_data);

    } else {

        tile_nrows = tile_payload.GetTileNRows();
        tile_ncols = tile_payload.GetTileNCols();

        UTILS_DIE("Not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    const BinMatDtype& bin_mat,
    const core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload;
    size_t payload_len;
    uint8_t* compressed_payload;
    size_t compressed_payload_len;

    auto tile_nrows = static_cast<uint32_t>(bin_mat.size());
    auto tile_ncols = static_cast<uint32_t>(bin_mat.empty() ? 0 : bin_mat[0].size());

    if (codec_ID == genie::core::AlgoID::JBIG) {

        bin_mat_to_bytes(bin_mat, &payload, payload_len);

        mpegg_jbig_compress_default(
            &compressed_payload,
            &compressed_payload_len,
            payload,
            payload_len,
            tile_nrows,
            tile_ncols
        );

        free(payload);

    } else {
        UTILS_DIE("Not yet implemented for other codec!");
    }

    auto _tile_payload = ContactMatrixTilePayload(
        codec_ID,
        tile_nrows,
        tile_ncols,
        &compressed_payload,
        compressed_payload_len
    );

    tile_payload = std::move(_tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

void conv_noop_on_sparse_mat(
    std::vector<uint64_t>& tile_row_ids,
    std::vector<uint64_t>& tile_col_ids,
    std::vector<uint32_t>& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output
){
    size_t num_entries = tile_counts.size();

    std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_tile_map;
    for (size_t i = 0u; i<num_entries; i++){
        auto lr_row_id = tile_row_ids[i] / bin_size_mult;
        auto lr_col_id = tile_col_ids[i] / bin_size_mult;
        auto count = tile_counts[i];

        lr_sparse_tile_map[{lr_row_id, lr_col_id}] += count;
    }

    size_t lr_num_entries = lr_sparse_tile_map.size();
    tile_row_ids.resize(lr_num_entries);
    tile_col_ids.resize(lr_num_entries);
    tile_counts.resize(lr_num_entries);

    size_t i_entry = 0u;
    for (const auto & pair_entry : lr_sparse_tile_map){
        tile_row_ids[i_entry] = (pair_entry.first).first;
        tile_col_ids[i_entry] = (pair_entry.first).second;
        tile_counts[i_entry] = pair_entry.second;
        i_entry++;
    }

    if (sort_output){
        sort_sparse_mat_inplace(
            tile_row_ids,
            tile_col_ids,
            tile_counts
        );
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult
){
    BinVecDtype row_mask;
    BinVecDtype col_mask;
    std::vector<uint64_t> start1_vec; // Use std::vector for collecting
    std::vector<uint64_t> end1_vec;
    std::vector<uint64_t> start2_vec;
    std::vector<uint64_t> end2_vec;
    std::vector<uint32_t> counts_vec;

    auto bin_size = cm_param.GetBinSize();
    auto target_bin_size = bin_size * bin_size_mult;
    auto tile_size = cm_param.GetTileSize();

    UTILS_DIE_IF(
        !cm_param.IsBinSizeMultiplierValid(bin_size_mult),
        "Bin size multiplier is invalid!"
    );

    auto chr1_ID = scm_param.GetChr1ID();
    auto chr2_ID = scm_param.GetChr2ID();
    auto is_intra_scm = scm_param.IsIntraSCM();

    auto codec_ID = scm_param.GetCodecID();
    bool row_mask_exists = scm_param.GetRowMaskExistsFlag();
    bool col_mask_exists = scm_param.GetColMaskExistsFlag();

    auto chr1_len = cm_param.GetChromosomeLength(chr1_ID);
    auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto chr2_len = cm_param.GetChromosomeLength(chr2_ID);
    auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
    auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

    if (row_mask_exists || col_mask_exists){
        decode_scm_masks(
            cm_param,
            scm_param,
            scm_payload,
            row_mask,
            col_mask
        );
    }

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
            if (i_tile > j_tile && is_intra_scm){
                continue;
            }

            UIntMatDtype tile_mat;
            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            UInt64VecDtype tile_row_ids;
            UInt64VecDtype tile_col_ids;
            UIntVecDtype tile_counts;

            auto& tile_param = scm_param.GetTileParameter(i_tile, j_tile);
            auto& tile_payload = scm_payload.GetTilePayload(i_tile, j_tile);
            auto binarization_mode = tile_param.binarization_mode;
            auto diag_transform_mode = tile_param.diag_tranform_mode;
            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

            if (tile_payload.GetPayloadSize() == 0){
                continue;
            }

            if (binarization_mode == BinarizationMode::ROW_BINARIZATION){
                BinMatDtype bin_mat;

                decode_cm_tile(
                    tile_payload,
                    codec_ID,
                    bin_mat
                );

                inverse_transform_row_bin(
                    bin_mat,
                    tile_mat
                );

            } else {
                UTILS_DIE("no binarization is not supported yet!");
            }

            inverse_diag_transform(
                tile_mat,
                diag_transform_mode
            );

            comp_start_end_ids(
                chr1_num_bin_entries,
                tile_size,
                i_tile,
                start1_idx,
                end1_idx
            );

            comp_start_end_ids(
                chr2_num_bin_entries,
                tile_size,
                j_tile,
                start2_idx,
                end2_idx
            );

            dense_to_sparse(
                tile_mat,
                tile_row_ids,
                tile_col_ids,
                tile_counts
            );

            if (row_mask_exists || col_mask_exists){
                BinVecDtype tile_row_mask(row_mask.begin() + start1_idx, row_mask.begin() + end1_idx);
                BinVecDtype tile_col_mask(col_mask.begin() + start2_idx, col_mask.begin() + end2_idx);

                insert_unaligned(
                    tile_row_ids,
                    tile_col_ids,
                    is_intra_tile,
                    tile_row_mask,
                    tile_col_mask
                );
            }

            for(auto& r_id : tile_row_ids) r_id += start1_idx;
            for(auto& c_id : tile_col_ids) c_id += start2_idx;

            if (bin_size_mult != 1){
                conv_noop_on_sparse_mat(
                    tile_row_ids,
                    tile_col_ids,
                    tile_counts,
                    bin_size_mult
                );
            }

            // Append to std::vectors
            start1_vec.insert(start1_vec.end(), tile_row_ids.begin(), tile_row_ids.end());
            start2_vec.insert(start2_vec.end(), tile_col_ids.begin(), tile_col_ids.end());
            counts_vec.insert(counts_vec.end(), tile_counts.begin(), tile_counts.end());
        }
    }

    auto sample_ID = scm_payload.GetSampleID();
    rec.SetSampleId(sample_ID);
    auto sample_name = std::string(cm_param.GetSampleName(sample_ID));
    rec.SetSampleName(std::move(sample_name));
    rec.SetChr1ID(chr1_ID);
    rec.SetChr2ID(chr2_ID);
    rec.SetBinSize(bin_size);
    
    end1_vec.resize(start1_vec.size());
    end2_vec.resize(start2_vec.size());

    for(size_t i=0; i<start1_vec.size(); ++i) {
        start1_vec[i] *= target_bin_size;
        end1_vec[i] = std::min(start1_vec[i] + target_bin_size, chr1_len);
        start2_vec[i] *= target_bin_size;
        end2_vec[i] = std::min(start2_vec[i] + target_bin_size, chr2_len);
    }
    
    rec.SetCMValues(
        std::move(start1_vec),
        std::move(end1_vec),
        std::move(start2_vec),
        std::move(end2_vec),
        std::move(counts_vec)
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    bool remove_unaligned_region,
    bool transform_mask,
    bool ena_diag_transform,
    bool ena_binarization,
    bool norm_as_weight,
    bool multiplicative_norm,
    core::AlgoID codec_ID
) {

  UTILS_DIE_IF(!ena_binarization, "Binarization must be activated!");

  BinVecDtype row_mask;
  BinVecDtype col_mask;

  auto interval = cm_param.GetBinSize();
  auto tile_size = cm_param.GetTileSize();
  auto num_entries = rec.GetNumEntries();
  auto chr1_ID = rec.GetChr1ID();
  auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
  auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
  auto chr2_ID = rec.GetChr2ID();
  auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
  auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

  cm_param.UpsertSample(rec.GetSampleID(), rec.GetSampleName());
  scm_payload.SetSampleID(rec.GetSampleID());

  scm_param.SetChr1ID(chr1_ID);
  scm_payload.SetChr1ID(chr1_ID);

  scm_param.SetChr2ID(chr2_ID);
  scm_payload.SetChr2ID(chr2_ID);

  auto is_intra_scm = scm_param.IsIntraSCM();

  scm_param.SetCodecID(codec_ID);

  scm_param.SetNumTiles(ntiles_in_row, ntiles_in_col);
  scm_payload.SetNumTiles(ntiles_in_row, ntiles_in_col);

  // Convert std::vector from record to xtensor
  std::vector<uint64_t> row_ids = rec.GetStartPos1();
  for(auto& id : row_ids) id /= interval;

  std::vector<uint64_t> col_ids = rec.GetStartPos2();
  for(auto& id : col_ids) id /= interval;

  std::vector<uint32_t> counts = rec.GetCounts();

  if (remove_unaligned_region){
      compute_masks(
          row_ids,
          col_ids,
          chr1_num_bin_entries,
          chr2_num_bin_entries,
          is_intra_scm,
          row_mask,
          col_mask
      );

      if (transform_mask){
          RunLengthEncodingData rowRLEData;
          set_rle_information_from_mask(rowRLEData, row_mask);
          
          if(is_intra_scm) {
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(
                rowRLEData.transformID,
                rowRLEData.firstVal,
                rowRLEData.rl_entries
            ));
            scm_param.SetRowMaskExistsFlag(true);
            scm_param.SetColMaskExistsFlag(false);
          } else {
            RunLengthEncodingData colRLEData;
            set_rle_information_from_mask(colRLEData, col_mask);
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(
                rowRLEData.transformID,
                rowRLEData.firstVal,
                rowRLEData.rl_entries
            ));
            scm_param.SetRowMaskExistsFlag(true);
            scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(
                colRLEData.transformID,
                colRLEData.firstVal,
                colRLEData.rl_entries
            ));
            scm_param.SetColMaskExistsFlag(true);
          }

      } else {
        // Direct mask storage, no RLE
        scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(std::move(row_mask)));
        scm_param.SetRowMaskExistsFlag(true);
        if (!is_intra_scm){
          scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(std::move(col_mask)));
          scm_param.SetColMaskExistsFlag(true);
        }
      }

      remove_unaligned(
          row_ids,
          col_ids,
          is_intra_scm,
          row_mask,
          col_mask
      );
  } else {
      scm_param.SetRowMaskExistsFlag(false);
      scm_param.SetColMaskExistsFlag(false);
  }

  for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
      for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
          if (i_tile > j_tile && is_intra_scm) {
              continue;
          }

          size_t start1_idx, end1_idx, start2_idx, end2_idx;
          comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
          comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);

          std::vector<uint64_t> tile_row_ids;
          std::vector<uint64_t> tile_col_ids;
          std::vector<uint32_t> tile_counts;

          for(size_t i=0; i<num_entries; ++i) {
              if (row_ids[i] >= start1_idx && row_ids[i] < end1_idx &&
                  col_ids[i] >= start2_idx && col_ids[i] < end2_idx) {
                  tile_row_ids.push_back(row_ids[i] - start1_idx);
                  tile_col_ids.push_back(col_ids[i] - start2_idx);
                  tile_counts.push_back(counts[i]);
              }
          }
          
          if (tile_counts.empty()) {
              scm_payload.SetTilePayload(
                  i_tile,
                  j_tile,
                  ContactMatrixTilePayload(codec_ID, 0, 0, nullptr, 0)
              );
              scm_param.SetTileParameter(
                  i_tile,
                  j_tile,
                  {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION}
              );
              continue;
          }
          
          std::vector<std::vector<uint32_t>> tile_mat;
          sparse_to_dense(
              tile_row_ids,
              tile_col_ids,
              tile_counts,
              tile_size,
              tile_size,
              tile_mat
          );

          if (ena_diag_transform) {
              diag_transform(tile_mat, DiagonalTransformMode::MODE_0);
          }
          
          std::vector<std::vector<bool>> bin_mat;
          if (ena_binarization) {
              transform_row_bin(tile_mat, bin_mat);
          } else {
              UTILS_DIE("Non-binarized encoding not supported for std!");
          }

          genie::contact::ContactMatrixTilePayload cm_tile_payload;
          encode_cm_tile(bin_mat, codec_ID, cm_tile_payload);

          scm_param.SetTileParameter(
              i_tile,
              j_tile,
              {
                  ena_diag_transform ? DiagonalTransformMode::MODE_0 : DiagonalTransformMode::NONE,
                  BinarizationMode::ROW_BINARIZATION
              }
          );
          scm_payload.SetTilePayload(i_tile, j_tile, std::move(cm_tile_payload));
      }
  }
}

} // namespace genie::contact::detail::std_lib

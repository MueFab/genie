#ifndef GENIE_CONTACT_CONTACT_CODER_STD_H
#define GENIE_CONTACT_CONTACT_CODER_STD_H

#include "contact_matrix_parameters.h"
#include "genie/core/constants.h"
#include "genie/core/contact_record/record.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"
#include "contact_types.h"
#include "genie/util/bit_reader.h"
#include <vector>

namespace genie {
namespace contact {
namespace detail::std_lib {

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const std::vector<bool>& scm_mask
);

void compute_mask(
    std::vector<uint64_t>& ids,
    size_t nelems,
    std::vector<bool>& mask
);

void compute_masks(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    std::vector<bool>& row_mask,
    std::vector<bool>& col_mask
);

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    std::vector<bool>& row_mask,
    std::vector<bool>& col_mask
);

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    std::vector<bool>& mask
);

void remove_unaligned(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    bool is_intra_tile,
    const std::vector<bool>& row_mask,
    const std::vector<bool>& col_mask
);

void insert_unaligned(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    bool is_intra_tile,
    const std::vector<bool>& row_mask,
    const std::vector<bool>& col_mask
);

void sparse_to_dense(
    const std::vector<uint64_t>& row_ids,
    const std::vector<uint64_t>& col_ids,
    const std::vector<uint32_t>& counts,
    size_t nrows,
    size_t ncols,
    std::vector<std::vector<uint32_t>>& mat
);

void dense_to_sparse(
    const std::vector<std::vector<uint32_t>>& mat,
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    std::vector<uint32_t>& counts
);

[[maybe_unused]] void sort_by_row_ids(
    std::vector<uint64_t>& row_ids,
    std::vector<uint64_t>& col_ids,
    std::vector<uint32_t>& counts
);

void inverse_diag_transform(
    std::vector<std::vector<uint32_t>>& mat,
    DiagonalTransformMode mode
);

void diag_transform(
    std::vector<std::vector<uint32_t>>& mat,
    DiagonalTransformMode mode
);

void inverse_transform_row_bin(
    const std::vector<std::vector<bool>>& bin_mat,
    std::vector<std::vector<uint32_t>>& mat
);

void transform_row_bin(
    const std::vector<std::vector<uint32_t>>& mat,
    std::vector<std::vector<bool>>& bin_mat
);

void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
);

void bin_mat_to_bytes(
    const std::vector<std::vector<bool>>& bin_mat,
    uint8_t** payload,
    size_t& payload_len
);

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    std::vector<std::vector<bool>>& bin_mat
);

void encode_cm_tile(
    const std::vector<std::vector<bool>>& bin_mat,
    core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
);

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    std::vector<std::vector<bool>>& bin_mat
);

void conv_noop_on_sparse_mat(
    std::vector<uint64_t>& tile_row_ids,
    std::vector<uint64_t>& tile_col_ids,
    std::vector<uint32_t>& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output=false
);

void sort_sparse_mat_inplace(
    std::vector<uint64_t>& tile_row_ids,
    std::vector<uint64_t>& tile_col_ids,
    std::vector<uint32_t>& tile_counts
);

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult
);

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
);

}
}
}

#endif //GENIE_CONTACT_CONTACT_CODER_STD_H

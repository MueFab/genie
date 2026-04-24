#ifndef GENIE_CONTACT_CONTACT_CODER_IMPL_H
#define GENIE_CONTACT_CONTACT_CODER_IMPL_H

#include "contact_matrix_parameters.h"
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"
#include "contact_types.h"
#include "genie/util/bit_reader.h"

namespace genie {
namespace contact {

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
);

void compute_mask(
    UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
);

void compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    BinVecDtype& mask
);

void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask,
    size_t row_offset,
    size_t row_size,
    size_t col_offset,
    size_t col_size,
    size_t& row_aligned_count,
    size_t& col_aligned_count
);

void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask,
    size_t row_offset,
    size_t row_size,
    size_t col_offset,
    size_t col_size
);

void sparse_to_dense(
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    UIntMatDtype& mat
);

void dense_to_sparse(
    const UIntMatDtype& mat,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

[[maybe_unused]] void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);

void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);

void inverse_transform_row_bin(
    const BinMatDtype& bin_mat,
    UIntMatDtype& mat
);

void transform_row_bin(
    const UIntMatDtype& mat,
    BinMatDtype& bin_mat
);

void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
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

void encode_cm_tile(
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
);

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
);

void conv_noop_on_sparse_mat(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output=false
);

void sort_sparse_mat_inplace(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
);

} // namespace contact
} // namespace genie

#endif //GENIE_CONTACT_CONTACT_CODER_IMPL_H

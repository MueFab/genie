#ifndef GENIE_CONTACT_CONTACT_CODER_EIGEN_H
#define GENIE_CONTACT_CONTACT_CODER_EIGEN_H

#include "contact_types.h"
#include "genie/core/contact_record/record.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"
#include "contact_matrix_parameters.h"
#include "genie/util/bit_reader.h"
#include <Eigen/Dense>
#include <vector>

namespace genie {
namespace contact {
namespace detail::eigen {

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const Eigen::Matrix<bool, -1, 1>& scm_mask
);

void compute_mask(
    const Eigen::Matrix<uint64_t, -1, 1>& ids,
    size_t nelems,
    Eigen::Matrix<bool, -1, 1>& mask
);

void compute_masks(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    Eigen::Matrix<bool, -1, 1>& row_mask,
    Eigen::Matrix<bool, -1, 1>& col_mask
);

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    Eigen::Matrix<bool, -1, 1>& row_mask,
    Eigen::Matrix<bool, -1, 1>& col_mask
);

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    Eigen::Matrix<bool, -1, 1>& mask
);

void remove_unaligned(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    bool is_intra_tile,
    const Eigen::Matrix<bool, -1, 1>& row_mask,
    const Eigen::Matrix<bool, -1, 1>& col_mask
);

void insert_unaligned(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    bool is_intra_tile,
    const Eigen::Matrix<bool, -1, 1>& row_mask,
    const Eigen::Matrix<bool, -1, 1>& col_mask
);

void sparse_to_dense(
    const Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    const Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    const Eigen::Matrix<uint32_t, -1, 1>& counts,
    size_t nrows,
    size_t ncols,
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat
);

void dense_to_sparse(
    const Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& counts
);

[[maybe_unused]] void sort_by_row_ids(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& counts
);

void inverse_diag_transform(
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    DiagonalTransformMode mode
);

void diag_transform(
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    DiagonalTransformMode mode
);

void inverse_transform_row_bin(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat
);

void transform_row_bin(
    const Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
);

void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
);

void bin_mat_to_bytes(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    uint8_t** payload,
    size_t& payload_len
);

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
);

void encode_cm_tile(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
);

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
);

void conv_noop_on_sparse_mat(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output=false
);

void sort_sparse_mat_inplace(
    Eigen::Matrix<uint64_t, -1, 1>& tile_row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& tile_col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& tile_counts
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

} // namespace detail::eigen
} // namespace contact
} // namespace genie

#endif // GENIE_CONTACT_CONTACT_CODER_EIGEN_H

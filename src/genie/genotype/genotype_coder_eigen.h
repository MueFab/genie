#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_EIGEN_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_EIGEN_H

#include <vector>
#include <cstdint>
#include <Eigen/Dense>
#include "genotype_parameters.h"
#include "genotype_payload.h"
#include "genie/core/variant_genotype_record/record.h"

namespace genie::genotype::detail::eigen {

// TODO: Define correct Eigen types
using BinMatDtype = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using UInt8MatDtype = Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using Int8MatDtype = Eigen::Matrix<int8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using UIntVecDtype = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;

void decompose(
    std::vector<core::record::VariantGenotype>& recs,
    uint8_t& max_ploidy,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat,
    size_t block_size
);

void transform_max_value(
    Int8MatDtype& allele_mat,
    bool& no_ref_flag,
    bool& not_avail_flag
);

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag);

void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    const ConcatAxis concat_axis
);

void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
);

void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
);

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mat,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
);

void binarize_allele_mat(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    UIntVecDtype& amax_vec,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis
);

void sort_matrix(
    BinMatDtype& bin_mat,
    const UIntVecDtype& ids,
    uint8_t axis
);

void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
);

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
);

void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
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

void entropy_encode_bin_mat(
    const BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    std::vector<uint8_t>& payload
);

void entropy_decode_bin_mat(
    const std::vector<uint8_t>& payload,
    genie::core::AlgoID codec_ID,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
);

void encode_and_sort_bin_mat(
    BinMatDtype& bin_mat,
    SortedBinMatPayload& sorted_bin_mat_payload,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
);

void decode_and_inverse_sort_bin_mat(
    const SortedBinMatPayload& sorted_bin_mat_payload,
    BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    bool sort_rows_flag,
    bool sort_cols_flag
);

void encode_genotype(
    std::vector<core::record::VariantGenotype>& recs,
    GenotypeParameters& params,
    GenotypePayload& payload,
    size_t block_size,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis,
    bool transpose_mat,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
);

void decode_genotype(
    const GenotypeParameters& params,
    const GenotypePayload& payload,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat
);

} // namespace genie::genotype::detail::eigen

#endif // GENIE_GENOTYPE_GENOTYPE_CODER_EIGEN_H

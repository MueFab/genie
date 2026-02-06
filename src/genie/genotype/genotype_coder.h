/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_H

#include <cstdint>
#include <vector>
#include <tuple>

#include "genie/core/constants.h"
#include "genie/core/variant_genotype_record/record.h"
#include "genotype_parameters.h"
#include "genotype_payload.h"
#include "genotype_types.h"

// Backend Selection
#if defined(GENIE_GENOTYPE_BACKEND_XTENSOR)
    #include "genotype_coder_xtensor.h"
    namespace genie::genotype {
        namespace backend = detail::xtensor;
    }
#elif defined(GENIE_GENOTYPE_BACKEND_EIGEN)
    #include "genotype_coder_eigen.h"
    namespace genie::genotype {
        namespace backend = detail::eigen;
    }
#elif defined(GENIE_GENOTYPE_BACKEND_STD)
    #include "genotype_coder_std.h"
    namespace genie::genotype {
        namespace backend = detail::std_lib;
    }
#else // Default to XTENSOR if nothing defined (or for legacy support)
    #define GENIE_GENOTYPE_BACKEND_XTENSOR
    #include "genotype_coder_xtensor.h"
    namespace genie::genotype {
        namespace backend = detail::xtensor;
    }
#endif

namespace genie::genotype {

struct EncodingOptions {
    uint32_t block_size = 0;
    BinarizationID binarization_ID = BinarizationID::UNDEFINED;
    ConcatAxis concat_axis = ConcatAxis::UNDEFINED;
    bool transpose_mat = false;
    SortingAlgoID sort_row_method = SortingAlgoID::NO_SORTING;
    SortingAlgoID sort_col_method = SortingAlgoID::NO_SORTING;
    genie::core::AlgoID codec_ID = genie::core::AlgoID::JBIG;
};

// Facade Wrappers

inline void decompose(
    std::vector<core::record::VariantGenotype>& recs,
    uint8_t& max_ploidy,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat,
    size_t block_size
) {
    backend::decompose(recs, max_ploidy, allele_mat, phasing_mat, block_size);
}

inline void transform_max_value(
    Int8MatDtype& allele_mat,
    bool& no_ref_flag,
    bool& not_avail_flag
) {
    backend::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
}

inline void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag) {
    backend::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
}

inline void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    const ConcatAxis concat_axis
) {
    backend::binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, concat_axis);
}

inline void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
) {
    backend::debinarize_bit_plane(bin_mats, num_bit_planes, concat_axis, allele_mat);
}

inline void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
) {
    backend::binarize_row_bin(allele_mat, bin_mats, amax_vec);
}

inline void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mat,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    backend::debinarize_row_bin(bin_mat, amax_vec, allele_mat);
}

inline void binarize_allele_mat(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    UIntVecDtype& amax_vec,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis
) {
    backend::binarize_allele_mat(allele_mat, bin_mats, num_bit_planes, amax_vec, binarization_ID, concat_axis);
}

inline void sort_matrix(
    BinMatDtype& bin_mat,
    const UIntVecDtype& ids,
    uint8_t axis
) {
    backend::sort_matrix(bin_mat, ids, axis);
}

inline void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
) {
    backend::random_sort_bin_mat(bin_mat, ids, axis);
}

inline void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
) {
    backend::sort_bin_mat(bin_mat, row_ids, col_ids, sort_row_method, sort_col_method);
}

inline void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
) {
    backend::invert_sort_bin_mat(bin_mat, row_ids, col_ids);
}

inline void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    backend::bin_mat_to_bytes(bin_mat, payload, payload_len);
}

inline void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    backend::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
}

inline void entropy_encode_bin_mat(
    const BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    std::vector<uint8_t>& payload
) {
    backend::entropy_encode_bin_mat(bin_mat, codec_ID, payload);
}

inline void entropy_decode_bin_mat(
    const std::vector<uint8_t>& payload,
    genie::core::AlgoID codec_ID,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    backend::entropy_decode_bin_mat(payload, codec_ID, nrows, ncols, bin_mat);
}

inline void encode_and_sort_bin_mat(
    BinMatDtype& bin_mat,
    SortedBinMatPayload& sorted_bin_mat_payload,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
) {
    backend::encode_and_sort_bin_mat(bin_mat, sorted_bin_mat_payload, sort_row_method, sort_col_method, codec_ID);
}

inline void decode_and_inverse_sort_bin_mat(
    const SortedBinMatPayload& sorted_bin_mat_payload,
    BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    bool sort_rows_flag,
    bool sort_cols_flag
) {
    backend::decode_and_inverse_sort_bin_mat(sorted_bin_mat_payload, bin_mat, codec_ID, sort_rows_flag, sort_cols_flag);
}

inline void encode_genotype(
    std::vector<core::record::VariantGenotype>& recs,
    GenotypeParameters& params,
    GenotypePayload& payload,
    size_t block_size=512,
    BinarizationID binarization_ID=BinarizationID::ROW_BIN,
    ConcatAxis concat_axis=ConcatAxis::DO_NOT_CONCAT,
    bool transpose_mat=false,
    SortingAlgoID sort_row_method=SortingAlgoID::NO_SORTING,
    SortingAlgoID sort_col_method=SortingAlgoID::NO_SORTING,
    genie::core::AlgoID codec_ID=genie::core::AlgoID::JBIG
) {
    backend::encode_genotype(recs, params, payload, block_size, binarization_ID, concat_axis, transpose_mat, sort_row_method, sort_col_method, codec_ID);
}

inline void decode_genotype(
    const GenotypeParameters& params,
    const GenotypePayload& payload,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat
) {
    backend::decode_genotype(params, payload, allele_mat, phasing_mat);
}

} // namespace genie::genotype

#endif // GENIE_GENOTYPE_GENOTYPE_CODER_H
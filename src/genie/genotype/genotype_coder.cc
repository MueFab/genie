/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cmath>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
#include <fstream>
#include <codecs/include/mpegg-codecs.h>
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/util/runtime-exception.h"
#include "genotype_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters generate_genotype_parameters(const EncodingOptions& opt, const EncodingBlock& block){
    auto num_bin_mat = getNumBinMats(block);
    std::vector<GenotypeBinMatParameters> variants_payload_params(num_bin_mat);
    for (auto i = 0u; i < num_bin_mat; i++){
        variants_payload_params[i].sort_rows_flag = opt.sort_row_method != SortingAlgoID::NO_SORTING;
        variants_payload_params[i].sort_cols_flag = opt.sort_col_method != SortingAlgoID::NO_SORTING;
        variants_payload_params[i].transpose_mat_flag = opt.transpose_mat;
        variants_payload_params[i].variants_codec_ID = opt.codec_ID;
    }

    auto unique_phasing_vals = xt::unique(block.phasing_mat);
    auto phase_value = unique_phasing_vals.at(0);
    bool encode_phases_data_flag = unique_phasing_vals.shape(0) > 1;
    GenotypeBinMatParameters phases_payload_param;
    if (encode_phases_data_flag) {
        phases_payload_param.sort_rows_flag = opt.sort_row_method != SortingAlgoID::NO_SORTING;
        phases_payload_param.sort_cols_flag = opt.sort_col_method != SortingAlgoID::NO_SORTING;
        phases_payload_param.transpose_mat_flag = opt.transpose_mat;
        phases_payload_param.variants_codec_ID = opt.codec_ID;
    }

    // TODO (Yeremia): Fix this
    auto params = GenotypeParameters(
        block.max_ploidy,
        block.dot_flag,
        block.na_flag,
        opt.binarization_ID,
        block.num_bit_planes,
        opt.concat_axis,
        std::move(variants_payload_params),
        encode_phases_data_flag,
        phases_payload_param,
        phase_value
    );

    return params;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t getNumBinMats(const EncodingBlock& block) { return static_cast<uint8_t>(block.allele_bin_mat_vect.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void decompose(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().getNumSamples();

    uint32_t i_rec = 0;
    auto& max_ploidy = block.max_ploidy;
    // Precompute max ploidy do avoid resizing during process
    for (i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.getNumSamples(), "Number of samples is not constant within a block!");

        auto tmp_p = static_cast<uint8_t>(rec.getNumberOfAllelesPerSample());
        max_ploidy = max_ploidy > tmp_p ? max_ploidy : tmp_p;
    }

    auto& allele_mat = block.allele_mat;

    MatShapeDtype allele_mat_shape = {block_size, num_samples * max_ploidy};
    allele_mat = xt::empty<bool, xt::layout_type::row_major>(allele_mat_shape);
    xt::view(allele_mat, xt::all(), xt::all()) = -2;  // Initialize value with "not available"

    auto& phasing_mat = block.phasing_mat;
    phasing_mat = BinMatDtype({block_size, num_samples * static_cast<uint8_t>(max_ploidy - 1)});

    sort_format(recs, block_size, block.attributeInfo, block.attributeData);

    for (i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];

        // Check and update num_samples;
        if (num_samples == 0) {
            num_samples = rec.getNumSamples();
        } else {
            UTILS_DIE_IF(num_samples != rec.getNumSamples(), "Number of samples is not constant within a block!");
        }

        auto& rec_alleles = rec.getAlleles();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            // Iterate only until given number of allele of the current record
            for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample(); k_allele++) {
                allele_mat(i_rec, j_sample * max_ploidy + k_allele) =
                    static_cast<int8_t>(rec_alleles[j_sample][k_allele]);
            }
        }

        if (max_ploidy - 1 > 0) {
            unsigned int phasing_ploidy = max_ploidy - 1;
            auto& rec_phasings = rec.getPhasing();
            for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
                // Iterate only until given number of allele of the current record
                for (uint8_t k_allele = 0; k_allele < rec.getNumberOfAllelesPerSample() - 1; k_allele++) {
                    phasing_mat(i_rec, j_sample * phasing_ploidy + k_allele) = rec_phasings[j_sample][k_allele];
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_max_value(
    Int8MatDtype& allele_mat,
    bool& no_ref_flag,
    bool& not_avail_flag
) {
    no_ref_flag = xt::any(xt::equal(allele_mat, -1));
    not_avail_flag = xt::any(xt::equal(allele_mat, -2));

    auto max_val = static_cast<int8_t>(xt::amax(allele_mat)(0));

    if (no_ref_flag) {
        max_val++;
        xt::filter(allele_mat, xt::equal(allele_mat, -1)) = max_val;
    }

    if (not_avail_flag) {
        max_val++;
        xt::filter(allele_mat, xt::equal(allele_mat, -2)) = max_val;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_max_val(
    Int8MatDtype& allele_mat,
    bool no_ref_flag,
    bool not_avail_flag
){
    auto max_val = static_cast<signed char>(xt::amax(allele_mat)(0));

    if (not_avail_flag) {
        xt::filter(allele_mat, xt::equal(allele_mat, max_val)) = -2;
        max_val--;
    }

    if (no_ref_flag) {
        xt::filter(allele_mat, xt::equal(allele_mat, max_val)) = -1;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    const ConcatAxis concat_axis,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes
) {
    auto max_val = xt::amax(allele_mat)(0);
    num_bit_planes = static_cast<uint8_t>(ceil(log2(max_val + 1)));

    bin_mats.resize(num_bit_planes);

    for (uint8_t k = 0; k < num_bit_planes; k++) {
        bin_mats[k] = xt::cast<bool>(allele_mat & (1 << k));
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
        for (uint8_t k = 1; k < num_bit_planes; k++) {
            if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
                bin_mats[0] = xt::concatenate(xt::xtuple(bin_mats[0], bin_mats[k]), 0);
            } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
                bin_mats[0] = xt::concatenate(xt::xtuple(bin_mats[0], bin_mats[k]), 1);
            }
        }
        // Clean-up the remaining bin_mats
        bin_mats.resize(1);
    }

    allele_mat.resize({0,0});
}

// ---------------------------------------------------------------------------------------------------------------------

void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    const uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
) {
    size_t nrows;
    size_t ncols;

    auto& bin_mat = bin_mats.front();
    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR){
        nrows = bin_mat.shape(0);
        auto new_nrows = nrows/num_bit_planes;
        for (auto k = 0u; k< num_bit_planes; k++){
            BinMatDtype curr_bin_mat = xt::view(bin_mat, xt::range((k)*new_nrows, (k+1)*new_nrows), xt::all());
            bin_mats.emplace_back(std::move(curr_bin_mat));
        }
    } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR){
        ncols = bin_mat.shape(1);
        auto new_ncols = ncols/num_bit_planes;
        for (auto k = 0u; k< num_bit_planes; k++){
            BinMatDtype curr_bin_mat = xt::view(bin_mat, xt::all(), xt::range((k)*new_ncols, (k+1)*new_ncols));
            bin_mats.emplace_back(std::move(curr_bin_mat));
        }
    }

    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR || concat_axis == ConcatAxis::CONCAT_COL_DIR){
        bin_mats.erase(bin_mats.begin());
    }

    auto num_bin_mats = bin_mats.size();
    nrows = bin_mats.front().shape(0);
    ncols = bin_mats.front().shape(1);
    allele_mat = xt::zeros<int8_t>({nrows, ncols});
    for (auto k = 0u; k<num_bin_mats; k++){
        allele_mat += xt::cast<int8_t>(bin_mats[k]) << k;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
) {
    auto nrows = allele_mat.shape(0);
    auto ncols = allele_mat.shape(1);

    amax_vec = xt::cast<uint8_t>(xt::ceil(xt::log2(xt::amax(allele_mat, 1) + 1)));
    auto new_nrows = static_cast<size_t>(xt::sum(amax_vec)(0));

    BinMatDtype bin_mat = xt::zeros<bool>({new_nrows, ncols});

    size_t i2 = 0;
    for (size_t i = 0; i < nrows; i++) {
        for (size_t i_bit = 0; i_bit < amax_vec[i]; i_bit++) {
            xt::view(bin_mat, i2++, xt::all()) = xt::cast<bool>(xt::view(allele_mat, i, xt::all()) & (1 << i_bit));
        }
    }

    bin_mats.emplace_back(std::move(bin_mat));

    allele_mat.resize({0,0});
}

// ---------------------------------------------------------------------------------------------------------------------

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    auto& bin_mat =  bin_mats.front();
    auto nrows = amax_vec.shape(0);
    auto ncols = bin_mat.shape(1);

    // Initialize values, has to be zero
    allele_mat = xt::zeros<int8_t>({nrows, ncols});

    int l = 0;
    for (auto i=0u; i<nrows; i++){
        for (auto j=0u; j< amax_vec[i]; j++){
            // Simplify the for-loop
            xt::view(allele_mat, i, xt::all()) += xt::cast<int8_t>(xt::view(bin_mat, l++, xt::all())) << j;
        }
    }

}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_allele_mat(
    Int8MatDtype& allele_mat,
    const BinarizationID binarization_ID,
    const ConcatAxis concat_axis,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    uint8_t& num_bit_planes
) {

    switch (binarization_ID) {
        case genie::genotype::BinarizationID::BIT_PLANE: {
            binarize_bit_plane(allele_mat, concat_axis, bin_mats, num_bit_planes);
            break;
        }
        case genie::genotype::BinarizationID::ROW_BIN: {
            binarize_row_bin(allele_mat, bin_mats, amax_vec);
            break;
        }
        default:
            UTILS_DIE("Invalid binarization_ID!");
    }

    // TODO(Yeremia): Free memory of allele_mat
    allele_mat.resize({0, 0});
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_matrix(
    BinMatDtype& bin_mat,
    const UIntVecDtype& ids,
    uint8_t axis
) {
    UTILS_DIE_IF(axis > 1, "Invalid axis value!");
    UTILS_DIE_IF(bin_mat.shape(axis) != ids.shape(0), "bin_mat and ids have different dimension!");

    if (ids.shape(0) == 0)
        return;

    // TODO(Yeremia): (optimization) Create a boolean vector for the buffer instead of whole matrix;
    BinMatDtype tmp_bin_mat = xt::empty_like(bin_mat);
    if (axis == 0) {
        for (auto i = 0u; i < ids.shape(0); i++) {
            xt::view(tmp_bin_mat, i, xt::all()) = xt::view(bin_mat, ids[i], xt::all());
        }
        for (uint32_t i = 0; i < ids.shape(0); i++) {
            xt::view(bin_mat, i, xt::all()) = xt::view(tmp_bin_mat, i, xt::all());
        }
    } else if (axis == 1) {
        for (auto i = 0u; i < ids.shape(0); i++) {
            xt::view(tmp_bin_mat, xt::all(), i) = xt::view(bin_mat, xt::all(), ids[i]);
        }
        for (auto i = 0u; i < ids.shape(0); i++) {
            xt::view(bin_mat, xt::all(), i) = xt::view(tmp_bin_mat, xt::all(), i);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void random_sort_bin_mat(BinMatDtype& bin_mat, UIntVecDtype& ids, uint8_t axis) {
    UTILS_DIE_IF(axis > 1, "Invalid axis value!");
    auto num_elem = static_cast<uint32_t>(bin_mat.shape(axis));
    // TODO(Yeremia): requires explicit conversion
    ids = xt::random::permutation<uint32_t>(num_elem);
    sort_matrix(bin_mat, ids, axis);
    ids = xt::argsort(ids);
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
) {
    switch (sort_row_method) {
        case genie::genotype::SortingAlgoID::NO_SORTING:
            row_ids = UIntVecDtype({0});
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            random_sort_bin_mat(bin_mat, row_ids, 0);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }

    switch (sort_col_method) {
        case genie::genotype::SortingAlgoID::NO_SORTING:
            col_ids = UIntVecDtype({0});
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            random_sort_bin_mat(bin_mat, col_ids, 1);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void invert_sort_bin_mat(BinMatDtype& bin_mat, UIntVecDtype& row_ids, UIntVecDtype& col_ids) {
    if (row_ids.shape(0) != 1) {
        genie::genotype::sort_matrix(bin_mat, row_ids, 0);
        row_ids.resize({0});
    }
    if (col_ids.shape(0) != 1) {
        genie::genotype::sort_matrix(bin_mat, col_ids, 1);

        col_ids.resize({0});
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_block(const EncodingOptions& opt, EncodingBlock& block) {
    auto num_bin_mats = getNumBinMats(block);

    block.allele_row_ids_vect.resize(num_bin_mats);
    block.allele_col_ids_vect.resize(num_bin_mats);
    for (uint8_t i_mat = 0; i_mat < num_bin_mats; i_mat++) {
        sort_bin_mat(
            block.allele_bin_mat_vect[i_mat],
            block.allele_row_ids_vect[i_mat],
            block.allele_col_ids_vect[i_mat],
            opt.sort_row_method,
            opt.sort_col_method
        );
    }

    // Sort phasing matrix
    sort_bin_mat(
        block.phasing_mat,
        block.phasing_row_ids,
        block.phasing_col_ids,
        opt.sort_row_method,
        opt.sort_col_method
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_to_bytes(BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len) {
    auto nrows = static_cast<size_t>(bin_mat.shape(0));
    auto ncols = static_cast<size_t>(bin_mat.shape(1));

    auto bpl = (ncols >> 3) + ((ncols & 7) > 0);  // Ceil operation
    payload_len = bpl * nrows;
    *payload = (unsigned char*)calloc(payload_len, sizeof(unsigned char));

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3);
            uint8_t shift = (7 - (j & 7));
            auto val = static_cast<uint8_t>(bin_mat(i, j));
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }

    //    auto* payload_ptr = *payload;
    //        size_t byte_offset = 0;
    //            auto byte_offset = static_cast<size_t>(floor(static_cast<double>(j)/8));
    //            auto rem_j = j % 8;
    //            auto bit_offset = (7 - (rem_j));
    //            auto mask = static_cast<uint8_t>(bin_mat(i, j));
    //            auto x = mask << bit_offset;
    //            *(payload_ptr + byte_offset) |= mask << bit_offset;
    //            byte_offset += (!rem_j && j > 0) ? 1 : 0;

    //            *(payload_ptr + byte_offset) |= bin_mat(i, j);
    //            *(payload_ptr + byte_offset) << 1;
    //            if (j % 8 && j > 0){
    //                byte_offset++;
    //            }
    //        payload_ptr += bpl;

    // SIMD version of the for loop
    //    MatShapeDtype buffer_mat_shape {nrows, bpl};
    ////    auto buffer_mat = UInt8MatDtype(buffer_mat_shape);
    //    UInt8MatDtype buffer_mat = xt::zeros<uint8_t>(buffer_mat_shape);
    ////    auto ncols_byte_aligned = static_cast<size_t>(floor(static_cast<double>(ncols) / 8));
    //    // Handle byte-aligned columns
    //    for (uint8_t ibit = 0; ibit < 8; ibit++){
    //        auto ith_bit_bin_mat = xt::view(bin_mat, xt::all(), xt::range(ibit, ncols_byte_aligned *8, 8));
    //        xt::view(buffer_mat, xt::all(), xt::range(0, ncols_byte_aligned)) |= xt::cast<uint8_t>(ith_bit_bin_mat) <<
    //        (7-ibit);
    //    }
    //    for (uint8_t ibit = 0; ibit < static_cast<uint8_t>(ncols % 8); ibit++){
    //        auto ith_bit_bin_mat = xt::view(bin_mat, xt::all(), xt::range(ncols_byte_aligned*8,
    //        ncols_byte_aligned*8+ibit)); xt::view(buffer_mat, xt::all(), ncols_byte_aligned) |=
    //        xt::cast<uint8_t>(ith_bit_bin_mat) << (7-ibit);
    //    }

    //    std::vector<double> w(a1.begin(), a1.end());
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(BinMatDtype& bin_mat, const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols) {
    auto bpl = (ncols >> 3) + ((ncols & 7) > 0);  // Ceil operation
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len / nrows / ncols!");

    MatShapeDtype bin_mat_shape = {nrows, ncols};
    bin_mat.resize(bin_mat_shape);
    xt::view(bin_mat, xt::all(), xt::all()) = false;  // Initialize value with 0

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3);
            uint8_t shift = (7 - (j & 7));
            bin_mat(i, j) = (*(payload + byte_offset) >> shift) & 1;
        }
    }
    //    auto* payload_ptr = payload;
    //        size_t byte_offset = 0;
    //        uint8_t val = 0;
    //            *(payload_ptr + byte_offset) |= bin_mat(i, j);
    //            *(payload_ptr + byte_offset) << 1;
    //            if (j % 8 && j > 0){
    //                byte_offset++;
    //            }
    //            auto rem_j = j % 8;
    //            auto bit_offset = (7 - (rem_j));
    //            auto val =  (*(payload_ptr + byte_offset) >> bit_offset);
    //            val &= 1;
    //            bin_mat(i, j) = val;
    //            byte_offset += (!rem_j && j > 0) ? 1 : 0;
    //            if (j % 8 == 0)
    //                val = *(payload_ptr + byte_offset);
    //
    //            bin_mat(i, j) = val & 1;
    //            val >>= 1;
    //            if (j % 8 && j > 0){
    //                byte_offset++;
    //            }
    //            *(payload + byte_offset) |= static_cast<uint8_t>(static_cast<uint8_t>(bin_mat(i, j)) << (7-(j%8)));
    //        }
    //        payload_ptr += bpl;
    //    }
}

// ---------------------------------------------------------------------------------------------------------------------
// TODO (Yeremia,Stefanie): Move and refactor this function to the parsing funciton

void sort_format(const std::vector<core::record::VariantGenotype>& recs, size_t block_size,
                 std::map<std::string, core::record::annotation_parameter_set::AttributeData>& info,
                 std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>>& values) {
    uint8_t AttributeID = 0;
    for (auto& attrInfo : info) {
        values[attrInfo.first].resize(block_size);
    }
    for (auto i_rec = 0u; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];
        
        for (const auto& format : rec.getFormats()) {
            values[format.getFormat()];
            if (values[format.getFormat()].empty()) values[format.getFormat()].resize(block_size);

            std::vector<std::vector<AttrType>> formatValue = format.getValue();
            if (values[format.getFormat()].at(i_rec).empty())
                values[format.getFormat()].at(i_rec) = formatValue;
            else
                values[format.getFormat()].at(i_rec).insert(values[format.getFormat()].at(i_rec).end(), formatValue.begin(), formatValue.end());
            info[format.getFormat()] = core::record::annotation_parameter_set::AttributeData(
                static_cast<uint8_t>(format.getFormat().size()), format.getFormat(), format.getType(),
                format.getArrayLength(), AttributeID);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void entropy_encode_bin_mat(
    BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    std::vector<uint8_t> payload
) {
    uint8_t* raw_data;
    size_t raw_data_len;
    uint8_t* compressed_data;
    size_t compressed_data_len;
    if (codec_ID == genie::core::AlgoID::BSC) {
    }

    bin_mat_to_bytes(bin_mat, &raw_data, raw_data_len);

    if (codec_ID == genie::core::AlgoID::JBIG) {
        mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, raw_data, raw_data_len,
                                    static_cast<unsigned long>(bin_mat.shape(0)),
                                    static_cast<unsigned long>(bin_mat.shape(1)));
    } else {
        UTILS_DIE("Invalid codec_ID for entropy coding!");
    }

    payload = std::vector<uint8_t>(compressed_data, compressed_data + compressed_data_len);

    free(raw_data);

    //#ifdef DEBUG

    unsigned long nrows;
    unsigned long ncols;

    mpegg_jbig_decompress_default(&raw_data, &raw_data_len, compressed_data, compressed_data_len, &nrows, &ncols);

    BinMatDtype recon_bin_mat;

    // recon_bin_mat must be initialized first with the correct size
    bin_mat_from_bytes(recon_bin_mat, raw_data, raw_data_len, nrows, ncols);

    //#endif

    free(compressed_data);

    UTILS_DIE_IF(bin_mat.shape() != recon_bin_mat.shape(), "Error");
    auto equality_check = xt::equal(bin_mat, recon_bin_mat);
    if (!xt::all(equality_check)) {
        for (auto i = 0u; i < nrows; i++) {
            for (auto j = 0u; j < ncols; j++) {
                auto val = equality_check(i, j);
                std::cerr << val;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<GenotypeParameters, EncodingBlock> encode_block(const EncodingOptions& opt,
                                                           std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(opt.binarization_ID == BinarizationID::UNDEFINED, "Invalid BinarizationID");
    UTILS_DIE_IF(opt.sort_row_method == SortingAlgoID::UNDEFINED, "Invalid SortingAlgoID");
    UTILS_DIE_IF(opt.sort_col_method == SortingAlgoID::UNDEFINED, "Invalid SortingAlgoID");

    EncodingBlock block{};
    decompose(opt, block, recs);
    transform_max_value(block.allele_mat, 
                        block.dot_flag, 
                        block.na_flag);
    binarize_allele_mat(block.allele_mat, 
                        opt.binarization_ID, 
                        opt.concat_axis, 
                        block.allele_bin_mat_vect,
                        block.amax_vec, 
                        block.num_bit_planes);

    genie::genotype::sort_block(opt, block);
    auto parameter = generate_genotype_parameters(opt, block);

    return {parameter, block};
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

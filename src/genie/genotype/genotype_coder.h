/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_GENOTYPE_CODER_H
#define GENIE_GENOTYPE_GENOTYPE_CODER_H

#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <tuple>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
// #include <xtensor/xmath.hpp>
// #include <xtensor/xoperation.hpp>
#pragma GCC diagnostic pop

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genotype_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

//using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
//using UInt8MatDtype = xt::xtensor<uint8_t, 2, xt::layout_type::row_major>;
using Int8MatDtype = xt::xtensor<int8_t, 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;

//using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;
using AttrType = std::vector<uint8_t>;

// ---------------------------------------------------------------------------------------------------------------------

enum class SortingAlgoID : uint8_t {
    NO_SORTING = 0,
    RANDOM_SORT = 1,
    NEAREST_NEIGHBOR = 2,
    LIN_KERNIGHAN_HEURISTIC = 3,
    UNDEFINED = 4
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size = 0;
    BinarizationID binarization_ID = BinarizationID::UNDEFINED;
    ConcatAxis concat_axis = ConcatAxis::UNDEFINED;
    bool transpose_mat = false;
    SortingAlgoID sort_row_method = SortingAlgoID::NO_SORTING;
    SortingAlgoID sort_col_method = SortingAlgoID::NO_SORTING;
    genie::core::AlgoID codec_ID = genie::core::AlgoID::JBIG;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    uint8_t max_ploidy = 0;
    bool dot_flag = false;
    bool na_flag = false;

    Int8MatDtype allele_mat;
    BinMatDtype phasing_mat;

    UIntVecDtype amax_vec;
    uint8_t num_bit_planes;
    std::vector<BinMatDtype> allele_bin_mat_vect;
    std::vector<UIntVecDtype> allele_row_ids_vect;
    std::vector<UIntVecDtype> allele_col_ids_vect;

    UIntVecDtype phasing_row_ids;
    UIntVecDtype phasing_col_ids;

    std::map<std::string, core::record::annotation_parameter_set::AttributeData> attributeInfo;

    // <block_size, num_samples, format_count/array_length, type>
    std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>> attributeData;

    likelihood::LikelihoodParameters likelihoodParameters;
    std::vector<uint32_t> likelihoodData;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Generates genotype parameters from an encoding block and options.
 *
 * Creates genotype parameters based on the given encoding options and block.
 * It sets up the parameters for variants and phases payload, including sorting and codec settings.
 *
 * @param opt The encoding options.
 * @param block The encoding block.
 * @return The generated genotype parameters.
 */
GenotypeParameters generate_genotype_parameters(const EncodingOptions& opt, const EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Gets the number of binary matrices in an encoding block.
 *
 * Returns the number of binary matrices stored in the encoding block.
 *
 * @param block The encoding block.
 * @return The number of binary matrices.
 */
uint8_t getNumBinMats(const EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decomposes records into an encoding block.
 *
 * Decomposes the given records into an encoding block, computing the maximum ploidy and creating allele and phasing matrices.
 *
 * @param opt The encoding options.
 * @param block The encoding block to decompose into.
 * @param recs The vector of variant genotype records to decompose.
 * @throws std::runtime_error If the number of records is empty or the number of samples is not constant within a block.
 */
void decompose(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::VariantGenotype>& recs);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Transforms the maximum value in an allele matrix.
 *
 * Checks for the presence of "no reference" and "not available" values in the allele matrix, and updates the maximum value accordingly.
 *
 * @param allele_mat The allele matrix to transform.
 * @param no_ref_flag Output flag indicating the presence of "no reference" values.
 * @param not_avail_flag Output flag indicating the presence of "not available" values.
 */
void transform_max_value(Int8MatDtype& allele_mat, bool& no_ref_flag, bool& not_avail_flag);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Inverse transforms the maximum value in an allele matrix.
 *
 * Reverts the transformation of the maximum value in the allele matrix, restoring "no reference" and "not available" values.
 *
 * @param allele_mat The allele matrix to inverse transform.
 * @param no_ref_flag Flag indicating the presence of "no reference" values.
 * @param not_avail_flag Flag indicating the presence of "not available" values.
 */
void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Binarizes an allele matrix into bit planes.
 *
 * Binarizes the allele matrix into multiple bit planes based on the maximum value, and concatenates them according to the specified axis.
 *
 * @param allele_mat The allele matrix to binarize.
 * @param concat_axis The axis to concatenate the bit planes.
 * @param bin_mats The output vector of binary matrices.
 * @param num_bit_planes The output number of bit planes.
 */
void binarize_bit_plane(Int8MatDtype& allele_mat, ConcatAxis concat_axis, std::vector<BinMatDtype>& bin_mats, uint8_t& num_bit_planes);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Debinarizes bit planes into an allele matrix.
 *
 * Debinarizes the bit planes into a single allele matrix, reversing the binarization process.
 *
 * @param bin_mats The input vector of binary matrices.
 * @param num_bit_planes The number of bit planes.
 * @param concat_axis The axis used for concatenation.
 * @param allele_mat The output allele matrix.
 */
void debinarize_bit_plane(std::vector<BinMatDtype>& bin_mats, uint8_t num_bit_planes, ConcatAxis concat_axis,  Int8MatDtype& allele_mat);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Binarizes an allele matrix into a binary matrix by row.
 *
 * Binarizes the allele matrix into a single binary matrix, where each row is binarized separately.
 *
 * @param allele_mat The input allele matrix.
 * @param bin_mats The output vector of binary matrices.
 * @param amax_vec The output vector of maximum values for each row.
 */
void binarize_row_bin(Int8MatDtype& allele_mat, std::vector<BinMatDtype>& bin_mats, UIntVecDtype& amax_vec);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Debinarizes a binary matrix into an allele matrix by row.
 *
 * Debinarizes the binary matrix into an allele matrix, reversing the binarization process.
 *
 * @param bin_mats The input vector of binary matrices.
 * @param amax_vec The input vector of maximum values for each row.
 * @param allele_mat The output allele matrix.
 */
void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mat,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Binarizes an allele matrix based on the specified binarization ID.
 *
 * Binarizes the allele matrix into binary matrices using the specified binarization method.
 *
 * @param allele_mat The input allele matrix.
 * @param binarization_ID The binarization method to use.
 * @param concat_axis The axis to concatenate the bit planes (for BIT_PLANE binarization).
 * @param bin_mats The output vector of binary matrices.
 * @param amax_vec The output vector of maximum values for each row (for ROW_BIN binarization).
 * @param num_bit_planes The output number of bit planes (for BIT_PLANE binarization).
 * @throws std::runtime_error If the binarization ID is invalid.
 */
void binarize_allele_mat(
    Int8MatDtype& allele_mat,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    uint8_t& num_bit_planes
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sorts a binary matrix based on the given IDs and axis.
 *
 * Sorts the binary matrix by rearranging its rows or columns according to the provided IDs.
 *
 * @param bin_mat The binary matrix to sort.
 * @param ids The IDs to sort by.
 * @param axis The axis to sort along (0 for rows, 1 for columns).
 * @throws std::runtime_error If the axis value is invalid or the dimensions of bin_mat and ids do not match.
 */
void sort_matrix(
    BinMatDtype& bin_mat,
    const UIntVecDtype& ids,
    uint8_t axis
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Randomly sorts a binary matrix along a specified axis.
 *
 * Generates a random permutation of IDs and sorts the binary matrix accordingly.
 *
 * @param bin_mat The binary matrix to sort.
 * @param ids The output IDs used for sorting.
 * @param axis The axis to sort along (0 for rows, 1 for columns).
 * @throws std::runtime_error If the axis value is invalid.
 */
void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sorts a binary matrix based on the specified sorting algorithms.
 *
 * Sorts the binary matrix by applying the specified sorting algorithms to its rows and columns.
 *
 * @param bin_mat The binary matrix to sort.
 * @param row_ids The output IDs for row sorting.
 * @param col_ids The output IDs for column sorting.
 * @param sort_row_method The sorting algorithm to use for rows.
 * @param sort_col_method The sorting algorithm to use for columns.
 * @throws std::runtime_error If the sorting method is invalid or not yet implemented.
 */
void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Inverts the sorting of a binary matrix.
 *
 * Reverts the sorting of the binary matrix by applying the inverse permutation to its rows and columns.
 *
 * @param bin_mat The binary matrix to invert sorting for.
 * @param row_ids The IDs to invert for row sorting.
 * @param col_ids The IDs to invert for column sorting.
 */
void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sorts an encoding block based on the specified options.
 *
 * Sorts the allele and phasing matrices in the encoding block according to the specified sorting algorithms.
 *
 * @param opt The encoding options.
 * @param block The encoding block to sort.
 */
void sort_block(
    const EncodingOptions& opt,
    EncodingBlock& block
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a binary matrix to a byte array.
 *
 * Packs the binary matrix into a byte array, where each byte represents 8 binary values.
 *
 * @param bin_mat The input binary matrix.
 * @param payload The output byte array.
 * @param payload_len The output length of the byte array.
 */
void bin_mat_to_bytes(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    uint8_t** payload,
    size_t& payload_len
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a byte array to a binary matrix.
 *
 * Unpacks the byte array into a binary matrix, where each byte represents 8 binary values.
 *
 * @param payload The input byte array.
 * @param payload_len The length of the byte array.
 * @param nrows The number of rows in the binary matrix.
 * @param ncols The number of columns in the binary matrix.
 * @param bin_mat The output binary matrix.
 * @throws std::runtime_error If the payload length is invalid.
 */
void bin_mat_from_bytes(
    // Inputs
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    // Outputs
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia,Stefanie): Move and refactor this function to the parsing function
void sort_format(
    const std::vector<core::record::VariantGenotype>& recs,
    size_t block_size,
    std::map<std::string,
    core::record::annotation_parameter_set::AttributeData>& info,
    std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>>& values
);

// ---------------------------------------------------------------------------------------------------------------------

void entropy_encode_bin_mat(
    BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    std::vector<uint8_t>& payload
);

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<GenotypeParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::VariantGenotype>& recs
);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_GENOTYPE_CODER_H

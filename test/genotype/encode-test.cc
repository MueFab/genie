/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <codecs/include/mpegg-codecs.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <vector>
#include <xtensor/xmath.hpp>
#include <xtensor/xoperation.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xnpy.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/entropy/jbig/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, Decompose) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO(Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);

    ASSERT_EQ(block.max_ploidy, 2);
    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);

    auto& allele_mat = block.allele_mat;
    auto& phasing_mat = block.phasing_mat;

    // Check allele_mat shape
    ASSERT_EQ(allele_mat.dimension(), 2);
    ASSERT_EQ(allele_mat.shape(0), 100);
    ASSERT_EQ(allele_mat.shape(1), 1092 * 2);
    ASSERT_EQ(allele_mat(0, 3), 1);

    // Check phasing_mat shape
    ASSERT_EQ(phasing_mat.dimension(), 2);
    ASSERT_EQ(phasing_mat.shape(0), 100);
    ASSERT_EQ(phasing_mat.shape(1), 1092);

    // Check all values
    ASSERT_EQ(xt::amin(allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(allele_mat)(0), 1);

    // Check the content of the first row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, 0, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, 0LL, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2067);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 117);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    // Check the content of the last row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, -1, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, -1LL, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2178);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 6);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_AdaptiveMaxValue) {
    size_t NROWS = 100;
    size_t NCOLS = 200;

    genie::genotype::Int8MatDtype allele_mat;
    genie::genotype::Int8MatDtype orig_allele_mat;
    genie::genotype::BinMatDtype mask;
    bool no_ref_flag;
    bool not_avail_flag;

    // Case 1: all positive
    {
        allele_mat = xt::random::randint<std::int16_t>({NROWS, NCOLS}, 0, 64);

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_FALSE(no_ref_flag);
        ASSERT_FALSE(not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);
    }

    // Case 2: no_ref
    {
     //   allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, 64);
   //     mask = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = -1;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_FALSE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == -1);
    }

    // Case 3: not_avail_flag
    {
    //    allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, 64);
    //    mask = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = -2;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_FALSE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == -2);
    }

    // Case 3: no_ref and not_avail_flag
    {
    //    allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, 64);
   //     mask = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = -1;
   //     mask = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = -2;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == -2);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_BinarizeBitPlane) {
    size_t NROWS = 100;
    size_t NCOLS = 200;
    int8_t MAX_ALLELE_VAL = 8;

    genie::genotype::Int8MatDtype allele_mat;
    genie::genotype::Int8MatDtype orig_allele_mat;
    std::vector<genie::genotype::BinMatDtype> bin_mats;
    uint8_t num_bin_mats;

    // Check DO_NOT_CONCAT
    {
   //     allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 3);
        ASSERT_EQ(num_bin_mats, 3);
    }

    // Check CONCAT_ROW_DIR
    {
   //     allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bin_mats, 3);
    }

    // Check CONCAT_COL_DIR
    {
   //     allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            genie::genotype::ConcatAxis::CONCAT_COL_DIR,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bin_mats, 3);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_BinarizeRowBin) {
    size_t NROWS = 100;
    size_t NCOLS = 200;
    int8_t MAX_ALLELE_VAL = 8;

    genie::genotype::Int8MatDtype allele_mat;
    genie::genotype::Int8MatDtype orig_allele_mat;
    std::vector<genie::genotype::BinMatDtype> bin_mats;
    genie::genotype::UIntVecDtype amax_vec;

    {
  //      allele_mat = xt::random::randint<int8_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::binarize_row_bin(
            allele_mat,
            bin_mats,
            amax_vec
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(amax_vec.shape(0), allele_mat.shape(0));
        ASSERT_EQ(xt::sum(amax_vec)(0), bin_mats.front().shape(0));

        genie::genotype::debinarize_row_bin(
            bin_mats,
            amax_vec,
            allele_mat
        );

        ASSERT_TRUE(allele_mat == orig_allele_mat);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_RandomSort) {
    size_t NROWS = 100;
    size_t NCOLS = 200;
    int8_t MAX_ALLELE_VAL = 2;

    genie::genotype::BinMatDtype bin_mat;
    genie::genotype::BinMatDtype orig_bin_mat;
    genie::genotype::UIntVecDtype row_ids;
    genie::genotype::UIntVecDtype col_ids;

    // Sort rows
    {
  //      bin_mat = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids,
            genie::genotype::SortingAlgoID::RANDOM_SORT,
            genie::genotype::SortingAlgoID::NO_SORTING
        );

        ASSERT_EQ(row_ids.shape(0), bin_mat.shape(0));
        ASSERT_EQ(col_ids.shape(0), 1);
        ASSERT_TRUE(bin_mat != orig_bin_mat);

        genie::genotype::invert_sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids
        );

        ASSERT_TRUE(bin_mat == orig_bin_mat);
    }

    // Sort cols
    {
    //    bin_mat = xt::cast<bool>(xt::random::randint<uint8_t>({NROWS, NCOLS}, 0, 2));
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids,
            genie::genotype::SortingAlgoID::NO_SORTING,
            genie::genotype::SortingAlgoID::RANDOM_SORT
        );

        ASSERT_EQ(row_ids.shape(0), 1);
        ASSERT_EQ(col_ids.shape(0), bin_mat.shape(1));
        ASSERT_TRUE(bin_mat != orig_bin_mat);

        genie::genotype::invert_sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids
        );

        ASSERT_TRUE(bin_mat == orig_bin_mat);
    }

    // Sort rows and cols
    {
        bin_mat = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids,
            genie::genotype::SortingAlgoID::RANDOM_SORT,
            genie::genotype::SortingAlgoID::RANDOM_SORT
        );

        ASSERT_EQ(row_ids.shape(0), bin_mat.shape(0));
        ASSERT_EQ(col_ids.shape(0), bin_mat.shape(1));
        ASSERT_TRUE(bin_mat != orig_bin_mat);

        genie::genotype::invert_sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids
        );

        ASSERT_TRUE(bin_mat == orig_bin_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, Serializer) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t bitmap[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2, 0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    size_t ncols = 23;
    size_t nrows = 5;

    genie::genotype::bin_mat_from_bytes(bin_mat, bitmap, orig_payload_len, nrows, ncols);

    std::vector<uint8_t> temp;
    for (auto byte : bin_mat) temp.push_back(byte);

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(bin_mat, &payload, payload_len);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(bitmap + i));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, JBIG) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t orig_payload[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    unsigned long orig_ncols = 23;
    unsigned long orig_nrows = 5;

    size_t orig_compressed_data_len = 37;
    uint8_t orig_compressed[37] = {0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
                                 0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2};

    uint8_t* compressed_data;
    size_t compressed_data_len;

    unsigned long ncols;
    unsigned long nrows;

    mpegg_jbig_compress_default(
        &compressed_data,
        &compressed_data_len,
        orig_payload,
        orig_payload_len,
        orig_nrows,
        orig_ncols
    );

    ASSERT_EQ(orig_compressed_data_len, compressed_data_len);
    for (size_t i = 0; i < compressed_data_len; ++i) {
        EXPECT_EQ(orig_compressed[i], compressed_data[i]);
    }

    std::stringstream uncomressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : orig_payload) uncomressed_input.write((char*)&byte, 1);
    genie::entropy::jbig::JBIGEncoder encoder;
    encoder.encode(uncomressed_input, compressed_output, (uint32_t)orig_ncols, (uint32_t)orig_nrows);

    std::vector<uint8_t> mem_data;
    for (auto byte : compressed_output.str()) {
        mem_data.push_back(static_cast<unsigned char>(byte));
    }
    for (size_t i = 0; i < compressed_data_len; ++i) {
        EXPECT_EQ(orig_compressed[i], mem_data[i]);
    }

    std::stringstream uncompressed_output;
    uint32_t output_ncols = 0;
    uint32_t output_nrows = 0;
    encoder.decode(compressed_output, uncompressed_output, output_ncols, output_nrows);

    std::vector<uint8_t> mem_data_source(3 * orig_payload_len);
    uint8_t* payload = &mem_data_source[0];
    size_t payload_len;

    mpegg_jbig_decompress_default(
        &payload,
        &payload_len,
        compressed_data,
        compressed_data_len,
        &nrows,
        &ncols
    );

    ASSERT_EQ(nrows, orig_nrows);
    ASSERT_EQ(ncols, orig_ncols);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(orig_payload + i));
    }
}

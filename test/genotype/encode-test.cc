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
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"

#include "genie/genotype/genotype_parameters.h"
#include "genie/entropy/jbig/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, Decompose) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
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
        genie::core::AlgoID::JBIG                    // codec_ID_;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);

    ASSERT_EQ(block.max_ploidy, 2u);
    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);

    auto& allele_mat = block.allele_mat;
    auto& phasing_mat = block.phasing_mat;

    // Check allele_mat shape
    ASSERT_EQ(allele_mat.dimension(), 2u);
    ASSERT_EQ(allele_mat.shape(0), 100u);
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
        auto allele_rec = xt::view(allele_mat, 0LL, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2067);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 117);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    // Check the content of the last row of allele_tensor
    {
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
    int8_t MAX_VAL = 64;
    int8_t NO_REF_VAL = -1;
    int8_t NOT_AVAIL_VAL = -2;

    genie::genotype::Int8MatDtype allele_mat;
    genie::genotype::Int8MatDtype orig_allele_mat;
    genie::genotype::BinMatDtype mask;
    bool no_ref_flag;
    bool not_avail_flag;

    // Case 1: all positive
    {
        allele_mat = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_VAL));

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
        allele_mat = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_VAL));
        mask = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = NO_REF_VAL;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_FALSE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == NO_REF_VAL);
    }

    // Case 3: not_avail_flag
    {
        allele_mat = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_VAL));
        mask = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = NOT_AVAIL_VAL;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_FALSE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == NOT_AVAIL_VAL);
    }

    // Case 3: no_ref and not_avail_flag
    {
        allele_mat = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_VAL));
        mask = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = NO_REF_VAL;
        mask = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
        xt::filter(allele_mat, mask) = NOT_AVAIL_VAL;

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(allele_mat == orig_allele_mat);
        ASSERT_TRUE(xt::amin(allele_mat)(0) == NOT_AVAIL_VAL);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_BinarizeBitPlane) {
    size_t NROWS = 100;
    size_t NCOLS = 200;
    int8_t MAX_ALLELE_VAL = 8;

    genie::genotype::Int8MatDtype allele_mat;
    std::vector<genie::genotype::BinMatDtype> bin_mats;
    uint8_t num_bin_mats;

    // Check DO_NOT_CONCAT
    {
        auto concat_axis_mode = genie::genotype::ConcatAxis::DO_NOT_CONCAT;
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL));

        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            concat_axis_mode,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 3);
        ASSERT_EQ(num_bin_mats, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        debinarize_bit_plane(bin_mats, num_bin_mats, concat_axis_mode, recon_allele_mat);

        ASSERT_TRUE(ORIG_ALLELE_MAT == recon_allele_mat);
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL));

        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bin_mats, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        debinarize_bit_plane(bin_mats, num_bin_mats, genie::genotype::ConcatAxis::CONCAT_ROW_DIR, recon_allele_mat);

        ASSERT_TRUE(ORIG_ALLELE_MAT == recon_allele_mat);
    }

    // Check CONCAT_COL_DIR
    {
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL));

        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(
            allele_mat,
            genie::genotype::ConcatAxis::CONCAT_COL_DIR,
            bin_mats,
            num_bin_mats
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bin_mats, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        debinarize_bit_plane(bin_mats, num_bin_mats, genie::genotype::ConcatAxis::CONCAT_COL_DIR, recon_allele_mat);

        ASSERT_TRUE(ORIG_ALLELE_MAT == recon_allele_mat);
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
        allele_mat = xt::cast<int8_t>(xt::random::randint<int16_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL));

        orig_allele_mat = allele_mat;

        genie::genotype::binarize_row_bin(
            allele_mat,
            bin_mats,
            amax_vec
        );

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(amax_vec.shape(0), NROWS);
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
        bin_mat = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, MAX_ALLELE_VAL));
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
        bin_mat = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2));
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

    size_t ORIG_PAYLOAD_LEN = 15;
    uint8_t ORIG_PAYLOAD[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2, 0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    size_t NCOLS = 23;
    size_t NROWS = 5;

    genie::genotype::BinMatDtype bin_mat;
    genie::genotype::bin_mat_from_bytes(
        ORIG_PAYLOAD,
        ORIG_PAYLOAD_LEN,
        NROWS,
        NCOLS,
        bin_mat
    );

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(
        bin_mat,
        &payload,
        payload_len
    );

    ASSERT_EQ(ORIG_PAYLOAD_LEN, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(ORIG_PAYLOAD + i));
    }

    free(payload);
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Move this test to JBIG
TEST(Genotype, RoundTrip_JBIG) {
//    genie::genotype::BinMatDtype bin_mat;
    size_t ORIG_PAYLOAD_LEN = 15;
    uint8_t ORIG_PAYLOAD[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    uint32_t ORIG_NCOLS = 23;
    uint32_t ORIG_NROWS = 5;

    size_t ORIG_COMPRESSED_PAYLOAD_LEN = 37;
    uint8_t ORIG_COMPRESSED_PAYLOAD[37] = {0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
                                 0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2};

    {
        uint8_t* compressed_data;
        size_t compressed_data_len;

        unsigned long ncols;
        unsigned long nrows;

        mpegg_jbig_compress_default(
            &compressed_data,
            &compressed_data_len,
            ORIG_PAYLOAD,
            ORIG_PAYLOAD_LEN,
            ORIG_NROWS,
            ORIG_NCOLS
        );

        ASSERT_EQ(ORIG_COMPRESSED_PAYLOAD_LEN, compressed_data_len);
        for (size_t i = 0; i < compressed_data_len; ++i) {
            EXPECT_EQ(ORIG_COMPRESSED_PAYLOAD[i], compressed_data[i]);
        }

        uint8_t* payload;
        size_t payload_len;

        mpegg_jbig_decompress_default(
            &payload,
            &payload_len,
            compressed_data,
            compressed_data_len,
            &nrows,
            &ncols
        );

        ASSERT_EQ(nrows, ORIG_NROWS);
        ASSERT_EQ(ncols, ORIG_NCOLS);

        ASSERT_EQ(ORIG_PAYLOAD_LEN, payload_len);
        for (size_t i = 0; i < payload_len; i++) {
            ASSERT_EQ(*(payload + i), *(ORIG_PAYLOAD + i)) << "index:" << i;
        }

        free(compressed_data);
        free(payload);
    }

    // TODO(stefanie): Fix this encoding-decoding using stringstream process
//    std::stringstream uncomressed_input;
//    std::stringstream compressed_output;
//    for (uint8_t byte : ORIG_PAYLOAD)
//        uncomressed_input.write((char*)&byte, 1);
//    genie::entropy::jbig::JBIGEncoder encoder;
//    encoder.encode(uncomressed_input, compressed_output, ORIG_NCOLS, ORIG_NROWS);
//
//    std::vector<uint8_t> mem_data;
//    for (auto byte : compressed_output.str()) {
//        mem_data.push_back(static_cast<unsigned char>(byte));
//    }
//    for (size_t i = 0; i < compressed_data_len; ++i) {
//        EXPECT_EQ(ORIG_COMPRESSED_PAYLOAD[i], mem_data[i]);
//    }
//
//    std::stringstream uncompressed_output;
//    uint32_t output_ncols = 0;
//    uint32_t output_nrows = 0;
//    encoder.decode(compressed_output, uncompressed_output, output_ncols, output_nrows);
//
//    std::vector<uint8_t> mem_data_source(3 * ORIG_PAYLOAD_LEN);
//    uint8_t* payload = &mem_data_source[0];
//    size_t payload_len;

}

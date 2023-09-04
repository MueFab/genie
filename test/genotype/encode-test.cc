/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <xtensor/xmath.hpp>
#include <xtensor/xoperation.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xview.hpp>
#include <codecs/include/mpegg-codecs.h>
#include "genie/util/bitwriter.h"
#include "genie/util/bitreader.h"
#include "genie/util/runtime-exception.h"
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/genotype_coder.h"
#include "helpers.h"

TEST(Genotype, Decompose) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.no_fmt.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    genie::genotype::EncodingOptions opt = {
        512,// block_size;
        genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
        false, // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::core::AlgoID::JBIG//codec_ID;
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
    ASSERT_EQ(allele_mat.shape(1), 1092*2);
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
        const auto& allele_rec = xt::view(allele_mat, 0, xt::all(), xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2067);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 117);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    // Check the content of the last row of allele_tensor
    {
        const auto& allele_rec = xt::view(allele_mat, -1, xt::all(), xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2178);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 6);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }
}

TEST(Genotype, AdaptiveMaxValue) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.no_fmt.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    genie::genotype::EncodingOptions opt = {
        512,// block_size;
        genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
        false, // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::core::AlgoID::JBIG//codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);

    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);
    ASSERT_EQ(xt::amin(block.allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(block.allele_mat)(0), 1);
}

TEST(Genotype, BinarizeBitPlane) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.no_fmt.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    // Check DO_NOT_CONCAT
    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::core::AlgoID::JBIG//codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::core::AlgoID::JBIG//codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_COL_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_COL_DIR, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::core::AlgoID::JBIG//codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }
}

TEST(Genotype, BinarizeRowBin) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.no_fmt.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    genie::genotype::EncodingOptions opt = {
        512,// block_size;
        genie::genotype::BinarizationID::ROW_BIN, // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
        false, // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
        genie::core::AlgoID::JBIG//codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);
    genie::genotype::binarize_allele_mat(opt, block);

    // TODO @Yeremia: Update the assert
    ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
    ASSERT_EQ(block.allele_bin_mat_vect[0].shape(0), static_cast<size_t>(xt::sum(block.amax_vec)(0)));
//    ASSERT_EQ(block.allele_bin_mat_vect.shape(2), block.allele_mat.shape(1));
}

TEST(Genotype, RandomSort) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.no_fmt.vcf.geno";

    std::ifstream reader(filepath);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT, // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::core::AlgoID::JBIG //codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING, // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT, // sort_row_method;
            genie::core::AlgoID::JBIG //codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,// block_size;
            genie::genotype::BinarizationID::BIT_PLANE, // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT, // concat_axis;
            false, // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT, // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT, // sort_row_method;
            genie::core::AlgoID::JBIG //codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }
}

TEST(Genotype, Serializer) {

    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t bitmap[15] = {
        0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
        0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38
    };
    size_t ncols = 23;
    size_t nrows = 5;

    genie::genotype::bin_mat_from_bytes(
        bin_mat,
        bitmap,
        orig_payload_len,
        nrows,
        ncols
    );

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(
        bin_mat,
        &payload,
        payload_len
    );

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i<payload_len; i++){
        ASSERT_EQ(*(payload+i), *(bitmap+i));
    }
}

TEST(Genotype, JBIG) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t orig_payload[15] = {
        0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
        0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38
    };
    unsigned long orig_ncols = 23;
    unsigned long orig_nrows = 5;

    uint8_t* compressed_data;
    size_t compressed_data_len;

    mpegg_jbig_compress_default(
        &compressed_data,
        &compressed_data_len,
        orig_payload,
        orig_payload_len,
        orig_nrows,
        orig_ncols
    );

    uint8_t* payload;
    size_t payload_len;

    unsigned long ncols;
    unsigned long nrows;
    mpegg_jbig_decompress_default(
        &payload,
        &payload_len,
        compressed_data,
        compressed_data_len,
        &nrows,
        &ncols);

    ASSERT_EQ(nrows, orig_nrows);
    ASSERT_EQ(ncols, orig_ncols);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i<payload_len; i++){
        ASSERT_EQ(*(payload+i), *(orig_payload +i));
    }
}

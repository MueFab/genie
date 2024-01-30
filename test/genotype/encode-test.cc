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
//#include <xtensor-io/xnpz.hpp>
//#include <xtensor-io/ximage.hpp>
//#include <xtensor-io/xaudio.hpp>
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

    //------------------------------------------------------
}


TEST(Genotype, AdaptiveMaxValue) {
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

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
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
    genie::genotype::transform_max_value(block);

    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);
    ASSERT_EQ(xt::amin(block.allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(block.allele_mat)(0), 1);
}

TEST(Genotype, BinarizeBitPlane) {
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

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    // Check DO_NOT_CONCAT
    {
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
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
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
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_COL_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
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
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::ROW_BIN,    // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
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

//TEST(Genotype, test_sort_matrix){
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::string python_data_path = gitRootDir + "/python/data";
//
//    auto NROWS = 50;
//    auto NCOLS = 100;
//
//    auto orig_bin_mat = xt::load_npy<bool>(python_data_path + "/orig_bin_mat.npy");
//    ASSERT_EQ(orig_bin_mat.dimension(), 2);
//    ASSERT_EQ(orig_bin_mat.shape(0), NROWS);
//    ASSERT_EQ(orig_bin_mat.shape(1), NCOLS);
//
//    {
//        genie::genotype::BinMatDtype row_sorted_bin_mat = xt::load_npy<bool>(python_data_path + "/row_sorted_bin_mat.npy");
//
//        ASSERT_EQ(row_sorted_bin_mat.dimension(), 2);
//        ASSERT_EQ(row_sorted_bin_mat.shape(0), NROWS);
//        ASSERT_EQ(row_sorted_bin_mat.shape(1), NCOLS);
//
//        auto row_ids = xt::load_npy<int64_t >(python_data_path + "/row_sorted_bin_mat.row_ids.npy");
//
//        ASSERT_EQ(row_ids.dimension(), 1);
//        ASSERT_EQ(row_ids.shape(0), NROWS);
//
//        for (auto k=0; k < NROWS; k++){
//            ASSERT_TRUE(
//                xt::view(orig_bin_mat, k, xt::all()) == xt::view(row_sorted_bin_mat, row_ids[k], xt::all())
//            ) << k;
//        }
//
//        genie::genotype::sort_matrix(row_sorted_bin_mat, row_ids, 0);
//
//        ASSERT_TRUE(
//            orig_bin_mat == row_sorted_bin_mat
//        );
//    }
//
//    {
//        genie::genotype::BinMatDtype col_sorted_bin_mat = xt::load_npy<bool>(python_data_path + "/col_sorted_bin_mat.npy");
//
//        ASSERT_EQ(col_sorted_bin_mat.dimension(), 2);
//        ASSERT_EQ(col_sorted_bin_mat.shape(0), NROWS);
//        ASSERT_EQ(col_sorted_bin_mat.shape(1), NCOLS);
//
//        auto col_ids = xt::load_npy<int64_t >(python_data_path + "/col_sorted_bin_mat.col_ids.npy");
//
//        ASSERT_EQ(col_ids.dimension(), 1);
//        ASSERT_EQ(col_ids.shape(0), NCOLS);
//
//        for (auto k=0; k < NCOLS; k++){
//            ASSERT_TRUE(
//                xt::view(orig_bin_mat, xt::all(), k) == xt::view(col_sorted_bin_mat, xt::all(), col_ids[k])
//                    ) << k;
//        }
//
//        genie::genotype::sort_matrix(col_sorted_bin_mat, col_ids, 1);
//
//        ASSERT_TRUE(
//            orig_bin_mat == col_sorted_bin_mat
//        );
//    }
//}

TEST(Genotype, RandomSort_Row) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_col_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        auto params = generate_genotype_parameters(opt, block);

        auto& allele_bin_mat_vect = block.allele_bin_mat_vect;
        std::vector<genie::genotype::BinMatDtype> orig_allele_bin_mat_vect;
        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++) {
            orig_allele_bin_mat_vect.emplace_back(allele_bin_mat_vect[k]);

//            ASSERT_EQ(orig_allele_bin_mat_vect.back().dimension(), 2);
//            ASSERT_EQ(orig_allele_bin_mat_vect.back().shape(0), 100);
        }

        genie::genotype::sort_block(opt, block);

        ASSERT_EQ(orig_allele_bin_mat_vect[0].dimension(), 2);
        ASSERT_EQ(orig_allele_bin_mat_vect[0].shape(0), 100);

//        xt::dump_npy(gitRootDir + "/orig_bin_mat.npy", orig_allele_bin_mat_vect[0]);
//        xt::dump_npy(gitRootDir + "/sorted_bin_mat.npy", allele_bin_mat_vect[0]);
//        xt::dump_npy(gitRootDir + "/sort_ids.npy", block.allele_row_ids_vect[0]);

        // Make sure that the result after sorting differs from the original
        for (size_t k = 0; k < allele_bin_mat_vect.size(); k++){
            ASSERT_FALSE(
                orig_allele_bin_mat_vect[k] == allele_bin_mat_vect[k]
            );
        }

        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++) {
            auto& orig_bin_mat = orig_allele_bin_mat_vect[k];
            auto& sorted_bin_mat = block.allele_bin_mat_vect[k];
            auto& sort_ids = block.allele_row_ids_vect[k];

            auto nrows = orig_bin_mat.shape(0);

            for (auto i = 0; k < nrows; k++) {
                ASSERT_TRUE(
                    xt::view(orig_bin_mat, i, xt::all()) ==
                    xt::view(sorted_bin_mat, sort_ids[i], xt::all())
                ) << k;
            }
        }

        genie::genotype::invert_sorted_block(params, block);

        // Make sure that the result after inverse transformation (un-sort) equals from the original
        allele_bin_mat_vect = block.allele_bin_mat_vect;
        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++){
            auto& orig_bin_mat = orig_allele_bin_mat_vect[k];
            auto& sorted_bin_mat = block.allele_bin_mat_vect[k];
            ASSERT_TRUE(
                orig_bin_mat == sorted_bin_mat
            ) << k;
        }
    }
}

TEST(Genotype, RandomSort_Col) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    {
        genie::genotype::EncodingOptions opt = {
            512,                                         // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
            false,                                       // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT, // sort_col_method;
            genie::core::AlgoID::JBIG                    // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        auto params = generate_genotype_parameters(opt, block);

        auto& allele_bin_mat_vect = block.allele_bin_mat_vect;
        std::vector<genie::genotype::BinMatDtype> orig_allele_bin_mat_vect;
        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++) {
            orig_allele_bin_mat_vect.emplace_back(allele_bin_mat_vect[k]);
        }

        genie::genotype::sort_block(opt, block);

        ASSERT_EQ(orig_allele_bin_mat_vect[0].dimension(), 2);
        ASSERT_EQ(orig_allele_bin_mat_vect[0].shape(0), 100);

        // Make sure that the result after sorting differs from the original
        for (size_t k = 0; k < allele_bin_mat_vect.size(); k++){
            ASSERT_FALSE(
                orig_allele_bin_mat_vect[k] == allele_bin_mat_vect[k]
            );
        }

        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++) {
            auto& orig_bin_mat = orig_allele_bin_mat_vect[k];
            auto& sorted_bin_mat = block.allele_bin_mat_vect[k];
            auto& sort_ids = block.allele_col_ids_vect[k];

            auto ncols = orig_bin_mat.shape(1);

            for (auto i = 0; k < ncols; k++) {
                ASSERT_TRUE(
                    xt::view(orig_bin_mat, xt::all(), i) ==
                    xt::view(sorted_bin_mat, xt::all(), sort_ids[i])
                        ) << k;
            }
        }

        genie::genotype::invert_sorted_block(params, block);

        // Make sure that the result after inverse transformation (un-sort) equals from the original
        for (size_t k = 0; k < genie::genotype::getNumBinMats(block); k++){
            auto& orig_bin_mat = orig_allele_bin_mat_vect[k];
            auto& sorted_bin_mat = block.allele_bin_mat_vect[k];
            ASSERT_TRUE(
                orig_bin_mat == sorted_bin_mat
                ) << k;
        }
    }
}

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

#include "genie/entropy/jbig/encoder.h"

TEST(Genotype, JBIG) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t orig_payload[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    unsigned long orig_ncols = 23;
    unsigned long orig_nrows = 5;

    uint8_t orig_compressed[] = {0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
                                 0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2};

    uint8_t* compressed_data;

    size_t compressed_data_len;

    mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, orig_payload, orig_payload_len, orig_nrows,
                                orig_ncols);

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
        mem_data.push_back(byte);
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

    unsigned long ncols;
    unsigned long nrows;
    mpegg_jbig_decompress_default(&payload, &payload_len, compressed_data, compressed_data_len, &nrows, &ncols);

    ASSERT_EQ(nrows, orig_nrows);
    ASSERT_EQ(ncols, orig_ncols);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(orig_payload + i));
    }
}

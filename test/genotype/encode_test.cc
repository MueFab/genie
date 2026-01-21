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

#include "genie/core/constants.h"
#include "genie/core/variant_genotype_record/record.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_test_helpers.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"

#include "genie/entropy/jbig/encoder.h"

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
        allele_mat = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_VAL);
        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_FALSE(no_ref_flag);
        ASSERT_FALSE(not_avail_flag);
        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == 0);
    }

    // Case 2: no_ref
    {
        allele_mat = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_VAL);
        mask = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        genie::genotype::set_element(mask, 0, 0, true); // Ensure at least one
        genie::genotype::set_by_mask(allele_mat, mask, NO_REF_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_FALSE(not_avail_flag);
        ASSERT_FALSE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == NO_REF_VAL);
    }

    // Case 3: not_avail_flag
    {
        allele_mat = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_VAL);
        mask = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        genie::genotype::set_element(mask, 0, 0, true); // Ensure at least one
        genie::genotype::set_by_mask(allele_mat, mask, NOT_AVAIL_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_FALSE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == NOT_AVAIL_VAL);
    }

    // Case 4: no_ref and not_avail_flag
    {
        allele_mat = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_VAL);
        mask = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        genie::genotype::set_element(mask, 0, 0, true); // Ensure at least one
        genie::genotype::set_by_mask(allele_mat, mask, NO_REF_VAL);
        mask = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        genie::genotype::set_element(mask, 0, 1, true); // Ensure at least one
        genie::genotype::set_by_mask(allele_mat, mask, NOT_AVAIL_VAL);

        orig_allele_mat = allele_mat;

        genie::genotype::transform_max_value(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(no_ref_flag);
        ASSERT_TRUE(not_avail_flag);
        ASSERT_FALSE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == 0);

        genie::genotype::inverse_transform_max_val(allele_mat, no_ref_flag, not_avail_flag);
        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
        ASSERT_TRUE(genie::genotype::amin(allele_mat) == NOT_AVAIL_VAL);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_BinarizeBitPlane) {
    size_t NROWS = 100;
    size_t NCOLS = 200;
    int8_t MAX_ALLELE_VAL = 8;

    genie::genotype::Int8MatDtype allele_mat;
    std::vector<genie::genotype::BinMatDtype> bin_mats;
    uint8_t num_bit_planes;

    // Check DO_NOT_CONCAT
    {
        auto concat_axis_mode = genie::genotype::ConcatAxis::DO_NOT_CONCAT;
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, concat_axis_mode);

        ASSERT_EQ(bin_mats.size(), 3);
        ASSERT_EQ(num_bit_planes, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        genie::genotype::debinarize_bit_plane(bin_mats, num_bit_planes, concat_axis_mode, recon_allele_mat);

        ASSERT_TRUE(genie::genotype::equal(ORIG_ALLELE_MAT, recon_allele_mat));
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, genie::genotype::ConcatAxis::CONCAT_ROW_DIR);

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bit_planes, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        genie::genotype::debinarize_bit_plane(bin_mats, num_bit_planes, genie::genotype::ConcatAxis::CONCAT_ROW_DIR, recon_allele_mat);

        ASSERT_TRUE(genie::genotype::equal(ORIG_ALLELE_MAT, recon_allele_mat));
    }

    // Check CONCAT_COL_DIR
    {
        genie::genotype::Int8MatDtype ORIG_ALLELE_MAT = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
        allele_mat = ORIG_ALLELE_MAT;

        genie::genotype::binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, genie::genotype::ConcatAxis::CONCAT_COL_DIR);

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(num_bit_planes, 3);

        genie::genotype::Int8MatDtype recon_allele_mat;
        genie::genotype::debinarize_bit_plane(bin_mats, num_bit_planes, genie::genotype::ConcatAxis::CONCAT_COL_DIR, recon_allele_mat);

        ASSERT_TRUE(genie::genotype::equal(ORIG_ALLELE_MAT, recon_allele_mat));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_BinarizeRowBin_BinaryMatrix) {
  size_t NROWS = 3;
  size_t NCOLS = 3;
  size_t BIN_NROWS = 3;

  genie::genotype::Int8MatDtype allele_mat;
  std::vector<genie::genotype::BinMatDtype> bin_mats;
  genie::genotype::UIntVecDtype amax_vec;

  genie::genotype::Int8MatDtype ALLELE_MAT = genie::genotype::create_matrix<int8_t>({{0, 0, 0},
                                              {1, 0, 1},
                                              {1, 0, 0}});

  allele_mat = ALLELE_MAT;

  genie::genotype::binarize_row_bin(allele_mat, bin_mats, amax_vec);

  ASSERT_EQ(bin_mats.size(), 1);
  ASSERT_EQ(genie::genotype::get_nrows(bin_mats[0]), BIN_NROWS);
  ASSERT_EQ(genie::genotype::get_ncols(bin_mats[0]), NCOLS);
  ASSERT_EQ(amax_vec.size(), NROWS);
  ASSERT_EQ(genie::genotype::sum(amax_vec), BIN_NROWS);

  genie::genotype::debinarize_row_bin(bin_mats, amax_vec, allele_mat);

  ASSERT_TRUE(genie::genotype::equal(allele_mat, ALLELE_MAT));
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
        allele_mat = genie::genotype::random_matrix<int8_t>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
        orig_allele_mat = allele_mat;

        genie::genotype::binarize_row_bin(allele_mat, bin_mats, amax_vec);

        ASSERT_EQ(bin_mats.size(), 1);
        ASSERT_EQ(amax_vec.size(), NROWS);
        ASSERT_EQ(genie::genotype::sum(amax_vec), genie::genotype::get_nrows(bin_mats.front()));

        genie::genotype::debinarize_row_bin(bin_mats, amax_vec, allele_mat);

        ASSERT_TRUE(genie::genotype::equal(allele_mat, orig_allele_mat));
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
        bin_mat = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(bin_mat, row_ids, col_ids, genie::genotype::SortingAlgoID::RANDOM_SORT, genie::genotype::SortingAlgoID::NO_SORTING);

        ASSERT_EQ(row_ids.size(), genie::genotype::get_nrows(bin_mat));
        ASSERT_EQ(col_ids.size(), 1);

        genie::genotype::invert_sort_bin_mat(bin_mat, row_ids, col_ids);
        ASSERT_TRUE(genie::genotype::equal(bin_mat, orig_bin_mat));
    }

    // Sort cols
    {
        bin_mat = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(bin_mat, row_ids, col_ids, genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::SortingAlgoID::RANDOM_SORT);

        ASSERT_EQ(row_ids.size(), 1);
        ASSERT_EQ(col_ids.size(), genie::genotype::get_ncols(bin_mat));

        genie::genotype::invert_sort_bin_mat(bin_mat, row_ids, col_ids);
        ASSERT_TRUE(genie::genotype::equal(bin_mat, orig_bin_mat));
    }

    // Sort rows and cols
    {
        bin_mat = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, 2);
        orig_bin_mat = bin_mat;

        genie::genotype::sort_bin_mat(
            bin_mat,
            row_ids,
            col_ids,
            genie::genotype::SortingAlgoID::RANDOM_SORT,
            genie::genotype::SortingAlgoID::RANDOM_SORT
        );

        ASSERT_EQ(row_ids.size(), genie::genotype::get_nrows(bin_mat));
        ASSERT_EQ(col_ids.size(), genie::genotype::get_ncols(bin_mat));

        genie::genotype::invert_sort_bin_mat(bin_mat, row_ids, col_ids);
        ASSERT_TRUE(genie::genotype::equal(bin_mat, orig_bin_mat));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, Serializer) {
    size_t ORIG_PAYLOAD_LEN = 15;
    uint8_t ORIG_PAYLOAD[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2, 0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    size_t NCOLS = 23;
    size_t NROWS = 5;

    genie::genotype::BinMatDtype bin_mat;
    genie::genotype::bin_mat_from_bytes(ORIG_PAYLOAD, ORIG_PAYLOAD_LEN, NROWS, NCOLS, bin_mat);

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(bin_mat, &payload, payload_len);

    ASSERT_EQ(ORIG_PAYLOAD_LEN, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(ORIG_PAYLOAD + i));
    }
    free(payload);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_JBIG) {
    size_t ORIG_PAYLOAD_LEN = 15;
    uint8_t ORIG_PAYLOAD[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    uint32_t ORIG_NCOLS = 23;
    uint32_t ORIG_NROWS = 5;

    size_t ORIG_COMPRESSED_PAYLOAD_LEN = 37;
    uint8_t ORIG_COMPRESSED_PAYLOAD[37] = {
      0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
      0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2
    };

    uint8_t* compressed_data;
    size_t compressed_data_len;
    unsigned long ncols, nrows;

    mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, ORIG_PAYLOAD, ORIG_PAYLOAD_LEN, ORIG_NROWS, ORIG_NCOLS);

    ASSERT_EQ(ORIG_COMPRESSED_PAYLOAD_LEN, compressed_data_len);
    for (size_t i = 0; i < compressed_data_len; ++i) {
        EXPECT_EQ(ORIG_COMPRESSED_PAYLOAD[i], compressed_data[i]);
    }

    uint8_t* payload;
    size_t payload_len;
    mpegg_jbig_decompress_default(&payload, &payload_len, compressed_data, compressed_data_len, &nrows, &ncols);

    ASSERT_EQ(nrows, ORIG_NROWS);
    ASSERT_EQ(ncols, ORIG_NCOLS);
    ASSERT_EQ(ORIG_PAYLOAD_LEN, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(ORIG_PAYLOAD + i)) << "index:" << i;
    }

    free(compressed_data);
    free(payload);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_EncodeAndSortBinMat) {
  size_t NROWS = 27;
  size_t NCOLS = 5;
  int8_t MAX_ALLELE_VAL = 2;

  genie::genotype::BinMatDtype ORIG_BIN_MAT = genie::genotype::random_matrix<bool>(NROWS, NCOLS, 0, MAX_ALLELE_VAL);
  genie::genotype::BinMatDtype bin_mat;

  const std::array<genie::genotype::SortingAlgoID, 2> sorting_methods = {
      genie::genotype::SortingAlgoID::NO_SORTING,
      genie::genotype::SortingAlgoID::RANDOM_SORT
  };

  const std::vector<genie::core::AlgoID> codecs = {
      genie::core::AlgoID::JBIG,
      genie::core::AlgoID::ZSTD,
      genie::core::AlgoID::BSC
  };

  for (const auto CODEC_ID : codecs) {
      for (const auto sort_row_method : sorting_methods) {
        for (const auto sort_col_method : sorting_methods) {
          auto sort_rows_flag = sort_row_method != genie::genotype::SortingAlgoID::NO_SORTING;
          auto sort_cols_flag = sort_col_method != genie::genotype::SortingAlgoID::NO_SORTING;

          bin_mat = ORIG_BIN_MAT;
          genie::genotype::SortedBinMatPayload sorted_bin_mat_payload;

          genie::genotype::encode_and_sort_bin_mat(bin_mat, sorted_bin_mat_payload, sort_row_method, sort_col_method, CODEC_ID);

          std::stringstream bitstream;
          genie::util::BitWriter writer(&bitstream);
          sorted_bin_mat_payload.Write(writer);

          std::istream& reader_st = bitstream;
          genie::util::BitReader bit_reader(reader_st);
          genie::genotype::SortedBinMatPayload recon_obj(bit_reader, CODEC_ID, sort_rows_flag, sort_cols_flag);

          ASSERT_TRUE(sorted_bin_mat_payload == recon_obj);

          genie::genotype::BinMatDtype recon_bin_mat;
          genie::genotype::decode_and_inverse_sort_bin_mat(recon_obj, recon_bin_mat, CODEC_ID, sort_rows_flag, sort_cols_flag);

          ASSERT_EQ(ORIG_BIN_MAT, recon_bin_mat);
        }
      }
  }
}

// -----------------------------------------------------------------------------

TEST(Genotype, RoundTrip_CASE12) {
  std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
  std::string filepath = gitRootDir + "/data/records/variant/1.3.5.header100.gt_only.vcf.geno";

  std::vector<genie::core::record::VariantGenotype> RECS;
  {
    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
      RECS.emplace_back(bitreader);
    }
    RECS.pop_back();
  }

  genie::genotype::GenotypeParameters params;
  genie::genotype::GenotypePayload payload;
  genie::genotype::encode_genotype(RECS, params, payload, 512, 
      genie::genotype::BinarizationID::BIT_PLANE, genie::genotype::ConcatAxis::CONCAT_ROW_DIR,
      false, genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::SortingAlgoID::NO_SORTING,
      genie::core::AlgoID::JBIG);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_JBIG_RandomMatrix) {
    const std::vector<std::pair<size_t, size_t>> dimensions = {{10, 10}, {100, 50}, {256, 256}};
    const std::vector<int8_t> max_vals = {1, 2, 3, 10, 64};
    auto codec = genie::core::AlgoID::JBIG;

    for (const auto& dim : dimensions) {
        for (int8_t max_val : max_vals) {
            genie::genotype::Int8MatDtype allele_mat = genie::genotype::random_matrix<int8_t>(dim.first, dim.second, 0, max_val);
            genie::genotype::Int8MatDtype original_allele_mat = allele_mat;
            genie::genotype::BinMatDtype phasing_mat = genie::genotype::random_matrix<bool>(dim.first, dim.second / 2 + 1, 0, 2);
            genie::genotype::BinMatDtype original_phasing_mat = phasing_mat;

            genie::genotype::GenotypePayload payload;
            genie::genotype::GenotypeParameters params(
                genie::genotype::BinarizationID::BIT_PLANE, genie::genotype::ConcatAxis::DO_NOT_CONCAT,
                false, false, false, codec, true, false, false, false, codec);

            bool dot_flag, na_flag;
            genie::genotype::transform_max_value(allele_mat, dot_flag, na_flag);
            payload.SetNoReferenceFlag(dot_flag);
            payload.SetNotAvailableFlag(na_flag);

            std::vector<genie::genotype::BinMatDtype> bin_mats;
            uint8_t num_bit_planes;
            genie::genotype::UIntVecDtype amax_vec;
            genie::genotype::binarize_allele_mat(allele_mat, bin_mats, num_bit_planes, amax_vec, params.GetBinarizationID(), params.GetConcatAxis());
            payload.SetNumBitPlanes(num_bit_planes);

            for (auto& bm : bin_mats) {
                genie::genotype::SortedBinMatPayload sbm;
                genie::genotype::encode_and_sort_bin_mat(bm, sbm, genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::SortingAlgoID::NO_SORTING, codec);
                payload.AddVariantsPayload(std::move(sbm));
            }

            genie::genotype::SortedBinMatPayload ph_sbm;
            genie::genotype::encode_and_sort_bin_mat(phasing_mat, ph_sbm, genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::SortingAlgoID::NO_SORTING, codec);
            payload.SetPhasesPayload(std::move(ph_sbm));

            genie::genotype::Int8MatDtype decoded_allele_mat;
            genie::genotype::BinMatDtype decoded_phasing_mat;
            genie::genotype::decode_genotype(params, payload, decoded_allele_mat, decoded_phasing_mat);

            ASSERT_TRUE(genie::genotype::equal(original_allele_mat, decoded_allele_mat));
            ASSERT_TRUE(genie::genotype::equal(original_phasing_mat, decoded_phasing_mat));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, RoundTrip_File_Integration) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::vector<std::string> files = {"/data/records/variant/1.3.5.header100.gt_only.vcf.geno"};

    for (const auto& file : files) {
        std::ifstream reader(gitRootDir + file, std::ios::binary);
        if (reader.fail()) continue;

        std::vector<genie::core::record::VariantGenotype> recs;
        genie::util::BitReader bitreader(reader);
        while (bitreader.IsStreamGood()) recs.emplace_back(bitreader);
        recs.pop_back();

        genie::genotype::GenotypeParameters params;
        genie::genotype::GenotypePayload payload;
        genie::genotype::encode_genotype(recs, params, payload, 512, genie::genotype::BinarizationID::BIT_PLANE, genie::genotype::ConcatAxis::DO_NOT_CONCAT, false, genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::SortingAlgoID::NO_SORTING, genie::core::AlgoID::JBIG);

        genie::genotype::Int8MatDtype decoded_allele_mat, expected_allele_mat;
        genie::genotype::BinMatDtype decoded_phasing_mat, expected_phasing_mat;
        genie::genotype::decode_genotype(params, payload, decoded_allele_mat, decoded_phasing_mat);

        uint8_t max_ploidy;
        genie::genotype::decompose(recs, max_ploidy, expected_allele_mat, expected_phasing_mat, 512);

        ASSERT_TRUE(expected_allele_mat == decoded_allele_mat) << "Allele Matrix Mismatch for file: " << file;
        ASSERT_TRUE(expected_phasing_mat == decoded_phasing_mat) << "Phasing Matrix Mismatch for file: " << file;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(Genotype, DISABLED_GenerateGoldenMaster) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string inputFile = "/data/records/variant/1.3.5.header100.gt_only.vcf.geno";
    std::string outputFile = gitRootDir + "/test/gold/genotype/ref_payload.bin";

    std::ifstream reader(gitRootDir + inputFile, std::ios::binary);
    ASSERT_FALSE(reader.fail()) << "Could not open input file: " << inputFile;

    std::vector<genie::core::record::VariantGenotype> recs;
    genie::util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }
    recs.pop_back();

    if (recs.empty()) return;

    genie::genotype::GenotypeParameters params;
    genie::genotype::GenotypePayload payload;
    
    // Fixed parameters for Golden Master
    genie::genotype::encode_genotype(
        recs,
        params,
        payload,
        512, // block size
        genie::genotype::BinarizationID::BIT_PLANE,
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,
        false,
        genie::genotype::SortingAlgoID::NO_SORTING,
        genie::genotype::SortingAlgoID::NO_SORTING,
        genie::core::AlgoID::JBIG
    );

    std::ofstream writer(outputFile, std::ios::binary);
    genie::util::BitWriter bitwriter(&writer);
    payload.Write(bitwriter);
    bitwriter.FlushBits();
    writer.close();
    
    std::cout << "Generated Golden Master at: " << outputFile << std::endl;
}

TEST(Genotype, CrossBackend_GoldenMaster) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string inputFile = "/data/records/variant/1.3.5.header100.gt_only.vcf.geno";
    std::string goldenFile = gitRootDir + "/test/gold/genotype/ref_payload.bin";

    // Read Golden Master
    std::ifstream goldenReader(goldenFile, std::ios::binary);
    ASSERT_FALSE(goldenReader.fail()) << "Could not open golden file: " << goldenFile;
    std::vector<uint8_t> goldenBytes((std::istreambuf_iterator<char>(goldenReader)), std::istreambuf_iterator<char>());

    // Read Input Records
    std::ifstream reader(gitRootDir + inputFile, std::ios::binary);
    ASSERT_FALSE(reader.fail()) << "Could not open input file: " << inputFile;

    std::vector<genie::core::record::VariantGenotype> recs;
    genie::util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }
    recs.pop_back();

    if (recs.empty()) return;

    genie::genotype::GenotypeParameters params;
    genie::genotype::GenotypePayload payload;
    
    // Encode with CURRENT backend
    genie::genotype::encode_genotype(
        recs,
        params,
        payload,
        512, // block size
        genie::genotype::BinarizationID::BIT_PLANE,
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,
        false,
        genie::genotype::SortingAlgoID::NO_SORTING,
        genie::genotype::SortingAlgoID::NO_SORTING,
        genie::core::AlgoID::JBIG
    );

    // Write to buffer
    std::stringstream buffer;
    genie::util::BitWriter bitwriter(&buffer);
    payload.Write(bitwriter);
    bitwriter.FlushBits();

    std::string generatedStr = buffer.str();
    std::vector<uint8_t> generatedBytes(generatedStr.begin(), generatedStr.end());

    ASSERT_EQ(goldenBytes.size(), generatedBytes.size()) << "Payload size mismatch!";
    ASSERT_EQ(goldenBytes, generatedBytes) << "Payload content mismatch!";
}

/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"
#include "genie/util/bitreader.h"
//#include "genie/util/bitwriter.h"
//#include "genie/util/runtime-exception.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, Simple_Coding_ComputeMask) {

    std::vector<uint64_t> IDS_VEC = {0, 1, 3, 5};
    auto IDS_NENTRIES = 8u;
    genie::contact::UInt64VecDtype IDS = xt::adapt(IDS_VEC, {IDS_VEC.size()});
    genie::contact::BinVecDtype mask;

    genie::contact::UInt64VecDtype orig_ids = IDS;

    genie::contact::compute_mask(IDS, IDS_NENTRIES, mask);

    ASSERT_EQ(mask.size(), IDS_NENTRIES);
    ASSERT_EQ(xt::sum(xt::cast<uint8_t>(mask))(0), 4u);
    ASSERT_TRUE(mask(0));
    ASSERT_TRUE(mask(5));
    ASSERT_FALSE(mask(2));
    ASSERT_FALSE(mask(4));

    // IDS shall not change
    ASSERT_TRUE(orig_ids == IDS);
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Create round trip test
TEST(ContactCoder, Simple_Coding_ComputeMasks) {
    // Intra SCM
    {
        auto IS_INTRA = true;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 2, 5};
        std::vector<uint64_t> COL_IDS_VEC = {1, 4, 6};
        auto NENTRIES = 8u;

        genie::contact::UInt64VecDtype row_ids = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids, NENTRIES,
            col_ids, NENTRIES,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(row_mask == col_mask);
        ASSERT_EQ(row_mask.size(), NENTRIES);
        ASSERT_EQ(col_mask.size(), NENTRIES);

        ASSERT_TRUE(row_mask(0));
        ASSERT_TRUE(row_mask(1));
        ASSERT_TRUE(row_mask(2));
        ASSERT_FALSE(row_mask(3));
        ASSERT_TRUE(row_mask(4));
        ASSERT_TRUE(row_mask(5));
        ASSERT_TRUE(row_mask(6));

        ASSERT_TRUE(col_mask(0));
        ASSERT_TRUE(col_mask(1));
        ASSERT_TRUE(col_mask(2));
        ASSERT_FALSE(col_mask(3));
        ASSERT_TRUE(col_mask(4));
        ASSERT_TRUE(col_mask(5));
        ASSERT_TRUE(col_mask(6));
    }

    // Inter case
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 2, 5};
        auto NROWS = 6u;
        std::vector<uint64_t> COL_IDS_VEC = {1, 4, 6};
        auto NCOLS = 8u;

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            ROW_IDS, NROWS,
            COL_IDS,
            NCOLS,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(row_mask != col_mask);
        ASSERT_EQ(row_mask.size(), NROWS);
        ASSERT_EQ(row_mask.dimension(), 1);
        ASSERT_EQ(col_mask.size(), NCOLS);
        ASSERT_EQ(col_mask.dimension(), 1);

        ASSERT_EQ(row_mask(0), true);
        ASSERT_EQ(row_mask(2), true);
        ASSERT_EQ(row_mask(5), true);
        ASSERT_EQ(row_mask(1), false);
        ASSERT_EQ(row_mask(3), false);
        ASSERT_EQ(row_mask(4), false);

        ASSERT_EQ(col_mask(1), true);
        ASSERT_EQ(col_mask(4), true);
        ASSERT_EQ(col_mask(6), true);
        ASSERT_EQ(col_mask(0), false);
        ASSERT_EQ(col_mask(2), false);
        ASSERT_EQ(col_mask(3), false);
        ASSERT_EQ(col_mask(5), false);
        ASSERT_EQ(col_mask(7), false);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_ProcessingUnalignedRegion) {
    // Intra SCM
    {
        auto IS_INTRA = true;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 5};
        std::vector<uint64_t> COL_IDS_VEC = {1, 6};
        size_t NENTRIES = 7u;

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = genie::contact::UInt64VecDtype(ROW_IDS);
        genie::contact::UInt64VecDtype col_ids = genie::contact::UInt64VecDtype(COL_IDS);

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids,
            NENTRIES,
            col_ids,
            NENTRIES,
            IS_INTRA,
            row_mask,
            col_mask
        );

        genie::contact::remove_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_ids(0), 0u);
        ASSERT_EQ(row_ids(1), 2u);
        ASSERT_EQ(col_ids(0), 1u);
        ASSERT_EQ(col_ids(1), 3u);

        genie::contact::insert_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_ids, ROW_IDS);
        ASSERT_EQ(col_ids, COL_IDS);
    }

    // Inter SCM
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 5};
        size_t NROWS = 6u;
        std::vector<uint64_t> COL_IDS_VEC = {1, 6};
        size_t NCOLS = 8u;

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = genie::contact::UInt64VecDtype(ROW_IDS);
        genie::contact::UInt64VecDtype col_ids = genie::contact::UInt64VecDtype(COL_IDS);

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids,
            NROWS,
            col_ids,
            NCOLS,
            IS_INTRA,
            row_mask,
            col_mask
        );
        genie::contact::remove_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_ids(0), 0u);
        ASSERT_EQ(row_ids(1), 1u);
        ASSERT_EQ(col_ids(0), 0u);
        ASSERT_EQ(col_ids(1), 1u);

        genie::contact::insert_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_ids, ROW_IDS);
        ASSERT_EQ(col_ids, COL_IDS);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_SparseDenseRepresentation) {
    // Test a matrix divided in 4 tiles
    // 5x5 scm matrix with tile size equals to 3

    // Tile (0,0)
    {
        auto ROW_ID_OFFSET = 0u;
        auto COL_ID_OFFSET = 0u;
        auto NROWS = 3u;
        auto NCOLS = 3u;

        std::vector<uint64_t> row_ids_vec = {0, 1, 2};
        std::vector<uint64_t> col_ids_vec = {1, 2, 0};
        std::vector<uint32_t> counts_vec = {1, 2, 3};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});
        genie::contact::UIntVecDtype counts = xt::adapt(counts_vec, {counts_vec.size()});

        genie::contact::sort_by_row_ids(
            row_ids,
            col_ids,
            counts
        );

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            counts,
            tile_mat,
            NROWS,
            NCOLS,
            ROW_ID_OFFSET,
            COL_ID_OFFSET
        );

        ASSERT_EQ(tile_mat.dimension(), 2);
        ASSERT_EQ(tile_mat.shape(0), 3);
        ASSERT_EQ(tile_mat.shape(1), 3);
        ASSERT_EQ(tile_mat(0, 1), 1);
        ASSERT_EQ(tile_mat(1, 2), 2);
        ASSERT_EQ(tile_mat(2, 0), 3);
        ASSERT_EQ(tile_mat(2, 2), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            ROW_ID_OFFSET,
            COL_ID_OFFSET,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::sort_by_row_ids(
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        ASSERT_TRUE(row_ids == recon_row_ids);
        ASSERT_TRUE(col_ids == recon_col_ids);
        ASSERT_TRUE(counts == recon_counts);
    }
    // Tile (1,0)
    {
        auto ROW_ID_OFFSET = 3u;
        auto COL_ID_OFFSET = 0u;
        auto NROWS = 2u;
        auto NCOLS = 3u;

        std::vector<uint64_t> row_ids_vec = {3, 4};
        std::vector<uint64_t> col_ids_vec = {2, 1};
        std::vector<uint32_t> counts_vec = {4, 5};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});
        genie::contact::UIntVecDtype counts = xt::adapt(counts_vec, {counts_vec.size()});

        genie::contact::sort_by_row_ids(
            row_ids,
            col_ids,
            counts
        );

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            counts,
            tile_mat,
            NROWS,
            NCOLS,
            ROW_ID_OFFSET,
            COL_ID_OFFSET
        );

        ASSERT_EQ(tile_mat.dimension(), 2);
        ASSERT_EQ(tile_mat.shape(0), 2);
        ASSERT_EQ(tile_mat.shape(1), 3);
        ASSERT_EQ(tile_mat(0, 2), 4);
        ASSERT_EQ(tile_mat(1, 1), 5);
        ASSERT_EQ(tile_mat(1, 2), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            ROW_ID_OFFSET,
            COL_ID_OFFSET,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::sort_by_row_ids(
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        ASSERT_TRUE(row_ids == recon_row_ids);
        ASSERT_TRUE(col_ids == recon_col_ids);
        ASSERT_TRUE(counts == recon_counts);
    }
    // Tile (0,1)
    {
        auto ROW_ID_OFFSET = 0u;
        auto COL_ID_OFFSET = 3u;
        auto NROWS = 3u;
        auto NCOLS = 2u;

        std::vector<uint64_t> row_ids_vec = {0, 2};
        std::vector<uint64_t> col_ids_vec = {4, 4};
        std::vector<uint32_t> counts_vec = {6, 7};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});
        genie::contact::UIntVecDtype counts = xt::adapt(counts_vec, {counts_vec.size()});

        genie::contact::sort_by_row_ids(
            row_ids,
            col_ids,
            counts
        );

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            counts,
            tile_mat,
            NROWS,
            NCOLS,
            ROW_ID_OFFSET,
            COL_ID_OFFSET
        );

        ASSERT_EQ(tile_mat.dimension(), 2);
        ASSERT_EQ(tile_mat.shape(0), 3);
        ASSERT_EQ(tile_mat.shape(1), 2);
        ASSERT_EQ(tile_mat(0, 1), 6);
        ASSERT_EQ(tile_mat(2, 1), 7);
        ASSERT_EQ(tile_mat(2, 0), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            ROW_ID_OFFSET,
            COL_ID_OFFSET,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::sort_by_row_ids(
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        ASSERT_TRUE(row_ids == recon_row_ids);
        ASSERT_TRUE(col_ids == recon_col_ids);
        ASSERT_TRUE(counts == recon_counts);
    }
    // Tile (1,1)
    {
        auto ROW_ID_OFFSET = 3u;
        auto COL_ID_OFFSET = 3u;
        auto NROWS = 2u;
        auto NCOLS = 2u;

        std::vector<uint64_t> row_ids_vec = {3};
        std::vector<uint64_t> col_ids_vec = {3};
        std::vector<uint32_t> counts_vec = {8};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});
        genie::contact::UIntVecDtype counts = xt::adapt(counts_vec, {counts_vec.size()});

        genie::contact::sort_by_row_ids(
            row_ids,
            col_ids,
            counts
        );

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            counts,
            tile_mat,
            NROWS,
            NCOLS,
            ROW_ID_OFFSET,
            COL_ID_OFFSET
        );

        ASSERT_EQ(tile_mat.dimension(), 2);
        ASSERT_EQ(tile_mat.shape(0), 2);
        ASSERT_EQ(tile_mat.shape(1), 2);
        ASSERT_EQ(tile_mat(0, 0), 8);
        ASSERT_EQ(tile_mat(0, 1), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            ROW_ID_OFFSET,
            COL_ID_OFFSET,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::sort_by_row_ids(
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        ASSERT_TRUE(row_ids == recon_row_ids);
        ASSERT_TRUE(col_ids == recon_col_ids);
        ASSERT_TRUE(counts == recon_counts);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Create round trip test
TEST(ContactCoder, diagonal_transformation) {
    // Mode 0
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_0;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 4},
                                            {0, 2, 3},
                                            {0, 0, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 2, 0},
                                                    {0, 3, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

    }
    // Mode 1 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 5},
                                            {4, 0, 3},
                                            {6, 0, 2}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 0, 2},
                                                    {0, 3, 4},
                                                    {0, 5, 6}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 1 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 4},
                                            {3, 0, 2}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 0, 0},
                                                    {2, 3, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 1 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 2},
                                            {3, 0},
                                            {4, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 0},
                                                    {2, 3},
                                                    {0, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }

    // Mode 2 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 6},
                                            {2, 0, 5},
                                            {1, 0, 4}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 2, 0},
                                                    {3, 0, 4},
                                                    {0, 5, 6}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 2 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{2, 0, 4},
                                            {1, 0, 3}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 2, 0},
                                                    {0, 3, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 2 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 4},
                                            {2, 0},
                                            {1, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 2},
                                                    {0, 3},
                                                    {0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }

    // Mode 3 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 1},
                                            {5, 0, 2},
                                            {6, 0, 4}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 0, 2},
                                                    {3, 0, 4},
                                                    {5, 0, 6}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 3 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 1},
                                            {4, 0, 2}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 0, 2},
                                                    {3, 0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
    // Mode 3 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{2, 1},
                                            {3, 0},
                                            {4, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = { {1, 2},
                                                    {0, 3},
                                                    {0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Binarization) {
    genie::contact::UIntMatDtype MAT = {{0, 0, 0},
                                        {1, 2, 3},
                                        {4, 5, 6}};
    genie::contact::BinMatDtype TARGET_BIN_MAT = {  {true, false, false, false},
                                                    {false, true, false, true},
                                                    {true, false, true, true},
                                                    {false, false, true, false},
                                                    {false, false, false, true},
                                                    {true, true, true, true}};

    genie::contact::UIntMatDtype orig_mat = genie::contact::UIntMatDtype(MAT);
    genie::contact::BinMatDtype bin_mat;
    genie::contact::binarize_row_bin(MAT, bin_mat);

    ASSERT_EQ(bin_mat, TARGET_BIN_MAT);

    genie::contact::UIntMatDtype recon_mat;
    //TODO(yeremia): create round-trip test for binarize_row_bin
//    genie::contact::debinarize_row_bin()
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Create round trip test
TEST(ContactCoder, RoundTrip_CodingOneRecNoNorm){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::ContactRecord> recs;

    while (bitreader.isGood()){
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    {
        auto TRANSFORM_MASK = false;
        auto TRANSFORM_TILE = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.setBinSize(recs.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: recs){
            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );
        }

        auto& rec = recs.front();
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            TRANSFORM_MASK,
            TRANSFORM_TILE,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), rec.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
//        auto recon_obj = genie::contact::SubcontactMatrixPayload(
//            bitreader,
//            cm_param
//        );

        auto scm_payload_len = scm_payload.getSize();
        auto x = 10;
    }

}

// ---------------------------------------------------------------------------------------------------------------------
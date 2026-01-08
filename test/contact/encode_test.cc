/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include "genie/contact/contact_coder.h"
#include "genie/contact/contact_test_helpers.h"
#include "genie/core/contact_record/record.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"
#include <iostream>
#include <filesystem>

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, Simple_Coding_ComputeMask) {

    std::vector<uint64_t> IDS_VEC = {0, 1, 3, 5};
    auto IDS_NENTRIES = 8u;
    auto IDS = genie::contact::create_vector(IDS_VEC);
    genie::contact::BinVecDtype mask;

    auto orig_ids = IDS;

    genie::contact::compute_mask(IDS, IDS_NENTRIES, mask);

    ASSERT_EQ(mask.size(), IDS_NENTRIES);
    ASSERT_EQ(genie::contact::sum(genie::contact::cast<uint8_t>(mask)), 4u);
    ASSERT_TRUE(genie::contact::get(mask, 0));
    ASSERT_TRUE(genie::contact::get(mask, 5));
    ASSERT_FALSE(genie::contact::get(mask, 2));
    ASSERT_FALSE(genie::contact::get(mask, 4));

    // IDS shall not change
    ASSERT_TRUE(genie::contact::equal(orig_ids, IDS));
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

        auto row_ids = genie::contact::create_vector(ROW_IDS_VEC);
        auto col_ids = genie::contact::create_vector(COL_IDS_VEC);

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids,
            col_ids,
            NENTRIES,
            NENTRIES,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(genie::contact::equal(row_mask, col_mask));
        ASSERT_EQ(row_mask.size(), NENTRIES);
        ASSERT_EQ(col_mask.size(), NENTRIES);

        ASSERT_TRUE(genie::contact::get(row_mask, 0));
        ASSERT_TRUE(genie::contact::get(row_mask, 1));
        ASSERT_TRUE(genie::contact::get(row_mask, 2));
        ASSERT_FALSE(genie::contact::get(row_mask, 3));
        ASSERT_TRUE(genie::contact::get(row_mask, 4));
        ASSERT_TRUE(genie::contact::get(row_mask, 5));
        ASSERT_TRUE(genie::contact::get(row_mask, 6));

        ASSERT_TRUE(genie::contact::get(col_mask, 0));
        ASSERT_TRUE(genie::contact::get(col_mask, 1));
        ASSERT_TRUE(genie::contact::get(col_mask, 2));
        ASSERT_FALSE(genie::contact::get(col_mask, 3));
        ASSERT_TRUE(genie::contact::get(col_mask, 4));
        ASSERT_TRUE(genie::contact::get(col_mask, 5));
        ASSERT_TRUE(genie::contact::get(col_mask, 6));
    }

    // Inter case
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 2, 5};
        auto NROWS = 6u;
        std::vector<uint64_t> COL_IDS_VEC = {1, 4, 6};
        auto NCOLS = 8u;

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            ROW_IDS,
            COL_IDS,
            NROWS,
            NCOLS,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_FALSE(genie::contact::equal(row_mask, col_mask));
        ASSERT_EQ(row_mask.size(), NROWS);
        ASSERT_EQ(col_mask.size(), NCOLS);

        ASSERT_EQ(genie::contact::get(row_mask, 0), true);
        ASSERT_EQ(genie::contact::get(row_mask, 2), true);
        ASSERT_EQ(genie::contact::get(row_mask, 5), true);
        ASSERT_EQ(genie::contact::get(row_mask, 1), false);
        ASSERT_EQ(genie::contact::get(row_mask, 3), false);
        ASSERT_EQ(genie::contact::get(row_mask, 4), false);

        ASSERT_EQ(genie::contact::get(col_mask, 1), true);
        ASSERT_EQ(genie::contact::get(col_mask, 4), true);
        ASSERT_EQ(genie::contact::get(col_mask, 6), true);
        ASSERT_EQ(genie::contact::get(col_mask, 0), false);
        ASSERT_EQ(genie::contact::get(col_mask, 2), false);
        ASSERT_EQ(genie::contact::get(col_mask, 3), false);
        ASSERT_EQ(genie::contact::get(col_mask, 5), false);
        ASSERT_EQ(genie::contact::get(col_mask, 7), false);
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

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);

        auto row_ids = ROW_IDS;
        auto col_ids = COL_IDS;

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids,
            col_ids,
            NENTRIES,
            NENTRIES,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(genie::contact::equal(row_mask, col_mask));

        genie::contact::remove_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(genie::contact::get(row_ids, 0), 0u);
        ASSERT_EQ(genie::contact::get(row_ids, 1), 2u);
        ASSERT_EQ(genie::contact::get(col_ids, 0), 1u);
        ASSERT_EQ(genie::contact::get(col_ids, 1), 3u);

        genie::contact::insert_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(genie::contact::equal(row_ids, ROW_IDS));
        ASSERT_TRUE(genie::contact::equal(col_ids, COL_IDS));
    }

    // Inter SCM
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 5};
        size_t NROWS = 6u;
        std::vector<uint64_t> COL_IDS_VEC = {1, 6};
        size_t NCOLS = 8u;

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);

        auto row_ids = ROW_IDS;
        auto col_ids = COL_IDS;

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(
            row_ids,
            col_ids,
            NROWS,
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

        ASSERT_EQ(genie::contact::get(row_ids, 0), 0u);
        ASSERT_EQ(genie::contact::get(row_ids, 1), 1u);
        ASSERT_EQ(genie::contact::get(col_ids, 0), 0u);
        ASSERT_EQ(genie::contact::get(col_ids, 1), 1u);

        genie::contact::insert_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_TRUE(genie::contact::equal(row_ids, ROW_IDS));
        ASSERT_TRUE(genie::contact::equal(col_ids, COL_IDS));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_SparseDenseRepresentation) {
    // Test a matrix divided in 4 tiles
    // 5x5 scm matrix with tile size equals to 3

    // Tile (0,0)
    {
        auto ROW_ID_OFFSET = 0u;
        auto COL_ID_OFFSET = 0u;
        auto NROWS = 3u;
        auto NCOLS = 3u;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 1, 2};
        std::vector<uint64_t> COL_IDS_VEC = {1, 2, 0};
        std::vector<uint32_t> COUNTS_VEC = {1, 2, 3};

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);
        auto COUNTS = genie::contact::create_vector(COUNTS_VEC);

        auto row_ids = genie::contact::subtract(ROW_IDS, ROW_ID_OFFSET);
        auto col_ids = genie::contact::subtract(COL_IDS, COL_ID_OFFSET);

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
        );

        ASSERT_EQ(genie::contact::rows(tile_mat), 3);
        ASSERT_EQ(genie::contact::cols(tile_mat), 3);
        ASSERT_EQ(genie::contact::get(tile_mat, 0, 1), 1);
        ASSERT_EQ(genie::contact::get(tile_mat, 1, 2), 2);
        ASSERT_EQ(genie::contact::get(tile_mat, 2, 0), 3);
        ASSERT_EQ(genie::contact::get(tile_mat, 2, 2), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids = genie::contact::add(recon_row_ids, ROW_ID_OFFSET);
        recon_col_ids = genie::contact::add(recon_col_ids, COL_ID_OFFSET);

        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(ROW_IDS), genie::contact::sort(recon_row_ids)));
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COL_IDS), genie::contact::sort(recon_col_ids)));
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COUNTS), genie::contact::sort(recon_counts)));
    }
    // Tile (1,0)
    {
        auto ROW_ID_OFFSET = 3u;
        auto COL_ID_OFFSET = 0u;
        auto NROWS = 2u;
        auto NCOLS = 3u;

        std::vector<uint64_t> ROW_IDS_VEC = {3, 4};
        std::vector<uint64_t> COL_IDS_VEC = {2, 1};
        std::vector<uint32_t> COUNTS_VEC = {4, 5};

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);
        auto COUNTS = genie::contact::create_vector(COUNTS_VEC);

        auto row_ids = genie::contact::subtract(ROW_IDS, ROW_ID_OFFSET);
        auto col_ids = genie::contact::subtract(COL_IDS, COL_ID_OFFSET);

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
        );

        ASSERT_EQ(genie::contact::rows(tile_mat), 2);
        ASSERT_EQ(genie::contact::cols(tile_mat), 3);
        ASSERT_EQ(genie::contact::get(tile_mat, 0, 2), 4);
        ASSERT_EQ(genie::contact::get(tile_mat, 1, 1), 5);
        ASSERT_EQ(genie::contact::get(tile_mat, 1, 2), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids = genie::contact::add(recon_row_ids, ROW_ID_OFFSET);
        recon_col_ids = genie::contact::add(recon_col_ids, COL_ID_OFFSET);

        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(ROW_IDS), genie::contact::sort(recon_row_ids)));
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COL_IDS), genie::contact::sort(recon_col_ids)));
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COUNTS), genie::contact::sort(recon_counts)));
    }
    // Tile (0,1)
    {
        auto ROW_ID_OFFSET = 0u;
        auto COL_ID_OFFSET = 3u;
        auto NROWS = 3u;
        auto NCOLS = 2u;

        std::vector<uint64_t> ROW_IDS_VEC = {0, 2};
        std::vector<uint64_t> COL_IDS_VEC = {4, 4};
        std::vector<uint32_t> COUNTS_VEC = {6, 7};

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);
        genie::contact::UIntVecDtype COUNTS = genie::contact::create_vector(COUNTS_VEC);

        genie::contact::UInt64VecDtype row_ids = genie::contact::subtract(ROW_IDS, ROW_ID_OFFSET);
        genie::contact::UInt64VecDtype col_ids = genie::contact::subtract(COL_IDS, COL_ID_OFFSET);

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
        );

        ASSERT_EQ(genie::contact::dims(tile_mat), 2);
        ASSERT_EQ(genie::contact::rows(tile_mat), 3);
        ASSERT_EQ(genie::contact::cols(tile_mat), 2);
        ASSERT_EQ(genie::contact::get(tile_mat, 0, 1), 6);
        ASSERT_EQ(genie::contact::get(tile_mat, 2, 1), 7);
        ASSERT_EQ(genie::contact::get(tile_mat, 2, 0), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::add_inplace(recon_row_ids, ROW_ID_OFFSET);
        genie::contact::add_inplace(recon_col_ids, COL_ID_OFFSET);

        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(ROW_IDS), genie::contact::sort(recon_row_ids))) << row_ids << recon_row_ids;
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COL_IDS), genie::contact::sort(recon_col_ids))) << col_ids << recon_col_ids;
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COUNTS), genie::contact::sort(recon_counts))) << COUNTS << recon_counts;
    }
    // Tile (1,1)
    {
        auto ROW_ID_OFFSET = 3u;
        auto COL_ID_OFFSET = 3u;
        auto NROWS = 2u;
        auto NCOLS = 2u;

        std::vector<uint64_t> ROW_IDS_VEC = {3};
        std::vector<uint64_t> COL_IDS_VEC = {3};
        std::vector<uint32_t> COUNTS_VEC = {8};

        auto ROW_IDS = genie::contact::create_vector(ROW_IDS_VEC);
        auto COL_IDS = genie::contact::create_vector(COL_IDS_VEC);
        genie::contact::UIntVecDtype COUNTS = genie::contact::create_vector(COUNTS_VEC);

        genie::contact::UInt64VecDtype row_ids = genie::contact::subtract(ROW_IDS, ROW_ID_OFFSET);
        genie::contact::UInt64VecDtype col_ids = genie::contact::subtract(COL_IDS, COL_ID_OFFSET);

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
        );

        ASSERT_EQ(genie::contact::dims(tile_mat), 2);
        ASSERT_EQ(genie::contact::rows(tile_mat), 2);
        ASSERT_EQ(genie::contact::cols(tile_mat), 2);
        ASSERT_EQ(genie::contact::get(tile_mat, 0, 0), 8);
        ASSERT_EQ(genie::contact::get(tile_mat, 0, 1), 0);

        genie::contact::UInt64VecDtype recon_row_ids;
        genie::contact::UInt64VecDtype recon_col_ids;
        genie::contact::UIntVecDtype recon_counts;

        genie::contact::dense_to_sparse(
            tile_mat,
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        genie::contact::add_inplace(recon_row_ids, ROW_ID_OFFSET);
        genie::contact::add_inplace(recon_col_ids, COL_ID_OFFSET);

        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(ROW_IDS), genie::contact::sort(recon_row_ids))) << row_ids << recon_row_ids;
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COL_IDS), genie::contact::sort(recon_col_ids))) << col_ids << recon_col_ids;
        ASSERT_TRUE(genie::contact::equal(genie::contact::sort(COUNTS), genie::contact::sort(recon_counts))) << COUNTS << recon_counts;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Create round trip test
TEST(ContactCoder, RoundTrip_Coding_DiagonalTransformation) {
    // None
    {
        auto MODE = genie::contact::DiagonalTransformMode::NONE;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 4},
                                                                {0, 2, 3},
                                                                {0, 0, 0}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }

    // Mode 0
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_0;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 4},
                                                                {0, 2, 3},
                                                                {0, 0, 0}});

        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 2, 0},
                                                                  {0, 3, 4}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 1 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 5},
                                                                {4, 0, 3},
                                                                {6, 0, 2}});

        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 2},
                                                                  {0, 3, 4},
                                                                  {0, 5, 6}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 1 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 4},
                                                                {3, 0, 2}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 0},
                                                                  {2, 3, 4}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 1 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{1, 2},
                                                                {3, 0},
                                                                {4, 0}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 0},
                                                                  {2, 3},
                                                                  {0, 4}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }

    // Mode 2 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{3, 0, 6},
                                                                {2, 0, 5},
                                                                {1, 0, 4}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 2, 0},
                                                                  {3, 0, 4},
                                                                  {0, 5, 6}});

        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 2 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{2, 0, 4},
                                                                {1, 0, 3}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 2, 0},
                                                                  {0, 3, 4}});
        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 2 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{3, 4},
                                                                {2, 0},
                                                                {1, 0}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 2},
                                                                  {0, 3},
                                                                  {0, 4}});
        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }

    // Mode 3 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{3, 0, 1},
                                                                {5, 0, 2},
                                                                {6, 0, 4}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 2},
                                                                  {3, 0, 4},
                                                                  {5, 0, 6}});
        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 3 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{3, 0, 1},
                                                                {4, 0, 2}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 0, 2},
                                                                  {3, 0, 4}});
        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
    // Mode 3 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        auto ORIG_MAT = genie::contact::create_matrix<uint32_t>({{2, 1},
                                                                {3, 0},
                                                                {4, 0}});
        auto TARGET_MAT = genie::contact::create_matrix<uint32_t>({{1, 2},
                                                                  {0, 3},
                                                                  {0, 4}});
        genie::contact::UIntMatDtype mat = ORIG_MAT;

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(genie::contact::equal(mat, TARGET_MAT));

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_CM_EQUAL(mat, ORIG_MAT);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_TransformRowBin) {
    auto MAT = genie::contact::create_matrix<uint32_t>({{0, 0, 0},
                                                       {1, 2, 3},
                                                       {4, 5, 6}});
    auto TARGET_BIN_MAT = genie::contact::create_matrix<bool>({  {true, false, false, false},
                                                               {false, true, false, true},
                                                               {true, false, true, true},
                                                               {false, false, true, false},
                                                               {false, false, false, true},
                                                               {true, true, true, true}});

    genie::contact::UIntMatDtype orig_mat = MAT;
    genie::contact::BinMatDtype bin_mat;
    genie::contact::transform_row_bin(orig_mat, bin_mat);

    ASSERT_EQ(genie::contact::rows(bin_mat), genie::contact::rows(TARGET_BIN_MAT));
    ASSERT_EQ(genie::contact::cols(bin_mat), genie::contact::cols(TARGET_BIN_MAT));
    ASSERT_CM_EQUAL(bin_mat, TARGET_BIN_MAT);

    genie::contact::UIntMatDtype recon_mat;
    genie::contact::inverse_transform_row_bin(bin_mat, recon_mat);

    ASSERT_EQ(genie::contact::rows(recon_mat), genie::contact::rows(MAT));
    ASSERT_EQ(genie::contact::cols(recon_mat), genie::contact::cols(MAT));
    ASSERT_CM_EQUAL(recon_mat, MAT);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_CodingCMTile) {
    // Test coding of bin_mat using JBIG
    {
        auto NROWS = 154u;
        auto NCOLS = 213u;
        auto CODEC_ID = genie::core::AlgoID::JBIG;

        genie::contact::BinMatDtype ORIG_BIN_MAT = genie::contact::cast<bool>(genie::contact::random_randint_mat<uint16_t>(NROWS, NCOLS, 0, 2u));

        auto bin_mat = genie::contact::BinMatDtype(ORIG_BIN_MAT);
        genie::contact::BinMatDtype recon_bin_mat;
        auto tile_payload = genie::contact::ContactMatrixTilePayload();

        encode_cm_tile(
            bin_mat,
            CODEC_ID,
            tile_payload
        );

        ASSERT_EQ(tile_payload.GetNumRows(), NROWS);
        ASSERT_EQ(tile_payload.GetNumCols(), NCOLS);

        decode_cm_tile(
            tile_payload,
            CODEC_ID,
            recon_bin_mat
        );

        ASSERT_EQ(recon_bin_mat, ORIG_BIN_MAT);
    }
    // Test coding of bin_mat using JBIG
    {
        auto NROWS = 2500u;
        auto NCOLS = 3750u;
        auto CODEC_ID = genie::core::AlgoID::JBIG;

        genie::contact::BinMatDtype ORIG_BIN_MAT = genie::contact::cast<bool>(genie::contact::random_randint_mat<uint16_t>(NROWS, NCOLS, 0, 2u));

        auto bin_mat = genie::contact::BinMatDtype(ORIG_BIN_MAT);
        genie::contact::BinMatDtype recon_bin_mat;
        auto tile_payload = genie::contact::ContactMatrixTilePayload();

        encode_cm_tile(
            bin_mat,
            CODEC_ID,
            tile_payload
        );

        ASSERT_EQ(tile_payload.GetNumRows(), NROWS);
        ASSERT_EQ(tile_payload.GetNumCols(), NCOLS);

        decode_cm_tile(
            tile_payload,
            CODEC_ID,
            recon_bin_mat
        );

        ASSERT_EQ(recon_bin_mat, ORIG_BIN_MAT);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_IntraSCM_Raw_SingleTile) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood() && reader.peek() != EOF) {
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    // One tiles - Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;
        auto NUM_TILES_IN_ROW = 1u;
        auto NUM_TILES_IN_COL = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), NUM_TILES_IN_ROW);
        ASSERT_EQ(scm_param.GetNTilesInRow(), NUM_TILES_IN_ROW);
        ASSERT_EQ(scm_payload.GetNTilesInCol(), NUM_TILES_IN_COL);
        ASSERT_EQ(scm_param.GetNTilesInCol(), NUM_TILES_IN_COL);

        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

            // One tiles - Case 02

            {

                auto REMOVE_UNALIGNED_REGION = true;

                auto TRANSFORM_MASK = false;

                auto ENA_DIAG_TRANSFORM = true;

                auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!

                bool NORM_AS_WEIGHT = true;

                bool MULTIPLICATIVE_NORM = true;

                auto CODEC_ID = genie::core::AlgoID::JBIG;

                auto TILE_SIZE = 1000u;

                auto MULT = 1u;

        

                auto cm_param = genie::contact::ContactMatrixParameters();

                auto scm_param = genie::contact::SubcontactMatrixParameters();

                auto scm_payload = genie::contact::SubcontactMatrixPayload();

        

                cm_param.SetBinSize(RECS.front().getBinSize());

                cm_param.SetTileSize(TILE_SIZE);

        

                for (auto& rec : RECS) {

                  cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

        

                    cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),

                                              rec.getChr1Length());

        

                    cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),

                                              rec.getChr2Length());

                }

        

                auto& REC = RECS.front();

                auto rec = genie::core::record::ContactRecord(REC);

                genie::contact::encode_scm(

                    cm_param,

                    rec,

                    scm_param,

                    scm_payload,

                    REMOVE_UNALIGNED_REGION,

                    TRANSFORM_MASK,

                    ENA_DIAG_TRANSFORM,

                    ENA_BINARIZATION,

                    NORM_AS_WEIGHT,

                    MULTIPLICATIVE_NORM,

                    CODEC_ID

                );

        

                auto obj_payload = std::stringstream();

                std::ostream& writer = obj_payload;

                auto bitwriter = genie::util::BitWriter(&writer);

                scm_payload.Write(bitwriter);

        

                ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());

                ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());

                ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());

                ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        

                std::istream& reader = obj_payload;

                auto bitreader = genie::util::BitReader(reader);

                auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        

                ASSERT_TRUE(recon_scm_payload == scm_payload);

        

                auto recon_rec = genie::core::record::ContactRecord();

        

                decode_scm(

                    cm_param,

                    scm_param,

                    recon_scm_payload,

                    recon_rec,

                    MULT

                );

        

    

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_IntraSCM_Raw_MultTiles){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    ASSERT_EQ(RECS.size(), 1);

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_IntraSCM_All_MultTiles){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-all-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    ASSERT_EQ(RECS.size(), 1);

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_IntraSCM_Raw_MultTiles_Downscale){

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_21.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_SingleTile) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood() && reader.peek() != EOF) {
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            genie::contact::UInt64VecDtype recon_start1 =
                genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            genie::contact::UInt64VecDtype recon_start2 =
                genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
//            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
            recon_end2 = genie::contact::sort(recon_end2);
            END2 = genie::contact::sort(END2);
            auto mask = genie::contact::equal(recon_end2, END2);
            ASSERT_TRUE(genie::contact::all(mask));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            genie::contact::UInt64VecDtype recon_start1 =
                genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            genie::contact::UInt64VecDtype recon_start2 =
                genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_SingleTiles_Downscale){

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;
        ASSERT_TRUE(std::filesystem::exists(filepath)) << filepath;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;
        ASSERT_TRUE(std::filesystem::exists(filepath)) << filepath;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        if (recon_rec.getNumEntries() != LR_REC.getNumEntries()){
            size_t recon_num_entries = recon_rec.getNumEntries();

            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            genie::contact::UInt64VecDtype recon_row_ids = recon_start1 / cm_param.GetBinSize() / MULT;

            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            genie::contact::UInt64VecDtype recon_col_ids = recon_start2 / cm_param.GetBinSize() / MULT;

            auto recon_counts = genie::contact::create_vector(recon_rec.getCounts());

            std::map<std::pair<uint64_t, uint64_t>, uint32_t> recon_sparse_mat;
            for (auto i_entry = 0u; i_entry<recon_num_entries; i_entry++){
                auto recon_row_id = recon_row_ids(i_entry);
                auto recon_col_id = recon_col_ids(i_entry);
                auto recon_count = recon_counts(i_entry);
                auto recon_row_col_id_pair = std::pair<uint64_t, uint64_t>(recon_row_id, recon_col_id);

                auto it = recon_sparse_mat.find(recon_row_col_id_pair);
                ASSERT_EQ(it, recon_sparse_mat.end());
                recon_sparse_mat.emplace(recon_row_col_id_pair, recon_count);
            }

            size_t lr_num_entries = LR_REC.getNumEntries();

            auto lr_start1 = genie::contact::create_vector(LR_REC.getStartPos1());
            genie::contact::UInt64VecDtype lr_row_ids = recon_start1 / cm_param.GetBinSize() / MULT;

            auto lr_start2 = genie::contact::create_vector(LR_REC.getStartPos2());
            genie::contact::UInt64VecDtype lr_col_ids = recon_start2 / cm_param.GetBinSize() / MULT;

            auto lr_counts = genie::contact::create_vector(LR_REC.getCounts());

            std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_mat;
            for (auto i_entry = 0u; i_entry<recon_num_entries; i_entry++){
                auto lr_row_id = lr_row_ids(i_entry);
                auto lr_col_id = lr_col_ids(i_entry);
                auto lr_count = lr_counts(i_entry);
                auto lr_row_col_id_pair = std::pair<uint64_t, uint64_t>(lr_row_id, lr_col_id);

                auto it = lr_sparse_mat.find(lr_row_col_id_pair);
                ASSERT_EQ(it, lr_sparse_mat.end());
                lr_sparse_mat.emplace(lr_row_col_id_pair, lr_count);
            }

            {
                size_t hr_num_entries = REC.getNumEntries();

                auto hr_start1 = genie::contact::create_vector(REC.getStartPos1());
                genie::contact::UInt64VecDtype hr_row_ids = recon_start1 / cm_param.GetBinSize() / MULT;

                auto hr_start2 = genie::contact::create_vector(REC.getStartPos2());
                genie::contact::UInt64VecDtype hr_col_ids = recon_start2 / cm_param.GetBinSize() / MULT;

                auto hr_counts = genie::contact::create_vector(REC.getCounts());

                std::vector<uint64_t> tmp_counts;
                for (auto i_entry = 0u; i_entry<hr_num_entries; i_entry++){
                    auto hr_row_id = hr_row_ids(i_entry);
                    auto hr_col_id = hr_col_ids(i_entry);
                    auto hr_count = hr_counts(i_entry);

                    if (hr_row_id == 64 && hr_col_id == 201){
                        tmp_counts.push_back(hr_count);
                    }
                }

                for (auto & lr_it : lr_sparse_mat) {
                    auto recon_it = recon_sparse_mat.find(lr_it.first);
                    ASSERT_NE(recon_it, recon_sparse_mat.end());
                    ASSERT_EQ(lr_it.second, recon_it->second) << "(" << lr_it.first.first << "," << lr_it.first.second << ")"
                              << tmp_counts[0] << "," << tmp_counts[1]  << "," << tmp_counts[2];
                }
            }


            // auto y = 10;
        }

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_MultTiles){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;
    ASSERT_TRUE(std::filesystem::exists(filepath)) << filepath;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }


    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            auto mask = genie::contact::not_equal(START1, recon_start1);
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

            cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                      rec.getChr1Length());

            cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                      rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_MultTiles_Downscale){

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;
        ASSERT_TRUE(std::filesystem::exists(filepath)) << filepath;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    }

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = false;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        if (recon_rec.getNumEntries() != LR_REC.getNumEntries()){
            size_t recon_num_entries = recon_rec.getNumEntries();

            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            genie::contact::UInt64VecDtype recon_row_ids = recon_start1 / cm_param.GetBinSize() / MULT;

            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            genie::contact::UInt64VecDtype recon_col_ids = recon_start2 / cm_param.GetBinSize() / MULT;

            auto recon_counts = genie::contact::create_vector(recon_rec.getCounts());

            std::map<std::pair<uint64_t, uint64_t>, uint32_t> recon_sparse_mat;
            for (auto i_entry = 0u; i_entry<recon_num_entries; i_entry++){
                auto recon_row_id = recon_row_ids(i_entry);
                auto recon_col_id = recon_col_ids(i_entry);
                auto recon_count = recon_counts(i_entry);
                auto recon_row_col_id_pair = std::pair<uint64_t, uint64_t>(recon_row_id, recon_col_id);

                auto it = recon_sparse_mat.find(recon_row_col_id_pair);
                ASSERT_EQ(it, recon_sparse_mat.end());
                recon_sparse_mat.emplace(recon_row_col_id_pair, recon_count);
            }

            size_t lr_num_entries = LR_REC.getNumEntries();

            auto lr_start1 = genie::contact::create_vector(LR_REC.getStartPos1());
            genie::contact::UInt64VecDtype lr_row_ids = recon_start1 / cm_param.GetBinSize() / MULT;

            auto lr_start2 = genie::contact::create_vector(LR_REC.getStartPos2());
            genie::contact::UInt64VecDtype lr_col_ids = recon_start2 / cm_param.GetBinSize() / MULT;

            auto lr_counts = genie::contact::create_vector(LR_REC.getCounts());

            std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_mat;
            for (auto i_entry = 0u; i_entry<recon_num_entries; i_entry++){
                auto lr_row_id = lr_row_ids(i_entry);
                auto lr_col_id = lr_col_ids(i_entry);
                auto lr_count = lr_counts(i_entry);
                auto lr_row_col_id_pair = std::pair<uint64_t, uint64_t>(lr_row_id, lr_col_id);

                auto it = lr_sparse_mat.find(lr_row_col_id_pair);
                ASSERT_EQ(it, lr_sparse_mat.end());
                lr_sparse_mat.emplace(lr_row_col_id_pair, lr_count);
            }

            for (auto & lr_it : lr_sparse_mat) {
                auto recon_it = recon_sparse_mat.find(lr_it.first);
                ASSERT_NE(recon_it, recon_sparse_mat.end());
                ASSERT_EQ(lr_it.second, recon_it->second) << "(" << lr_it.first.first << "," << lr_it.first.second << ")";
            }

            // auto y = 10;
        }

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = false;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 5u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);
        cm_param.UpsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        auto& LR_REC = LR_RECS.front();

        ASSERT_EQ(recon_rec.getNumEntries(), LR_REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(LR_REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(LR_REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(LR_REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(LR_REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(LR_REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_RLESubcontactMatrixPayload) {
    {   // Case: Test if the RLE information is correct
        std::vector<bool> test_vector = {true, true, true, true, false, true, false, false};
        auto test_rl_entries = genie::contact::create_vector<uint32_t>({4,1,1,2});
        genie::contact::BinVecDtype dummy_mask = genie::contact::create_vector(test_vector);
        genie::contact::RunLengthEncodingData test_rle_data;

        genie::contact::set_rle_information_from_mask(test_rle_data, dummy_mask);

        ASSERT_EQ(test_rle_data.firstVal, true);
        ASSERT_EQ(test_rle_data.rl_entries, test_rl_entries);
        ASSERT_EQ(test_rle_data.maxCount, 4);
        ASSERT_EQ(test_rle_data.transformID, genie::contact::TransformID::ID_1);
    }


    {
        // Case: scm_mask_payload with rle data
        std::vector<bool> test_vector = {true, true, true, true, false, true, false, false};
        auto test_rl_entries = genie::contact::create_vector<uint32_t>({4,1,1,2});
        genie::contact::BinVecDtype dummy_mask = genie::contact::create_vector(test_vector);

        // Get the corresponding RLE encoding data
        genie::contact::RunLengthEncodingData test_rle_data;
        genie::contact::set_rle_information_from_mask(test_rle_data, dummy_mask);

        // Create dummy scm_payload
        ASSERT_EQ(test_rle_data.firstVal, true);
        ASSERT_EQ(test_rle_data.rl_entries, test_rl_entries);
        ASSERT_EQ(test_rle_data.maxCount, 4);
        ASSERT_EQ(test_rle_data.transformID, genie::contact::TransformID::ID_1);

        // Configure transformID, FirstVal, etc. for scm_mask_payload
        auto test_scm_mask_payload = genie::contact::SubcontactMatrixMaskPayload();
        test_scm_mask_payload.SetMaskArray(dummy_mask);
        test_scm_mask_payload.SetRlEntries(test_rle_data.transformID,
                                           test_rle_data.firstVal,
                                           test_rle_data.rl_entries);

        // recast
        std::vector<uint32_t> rleEntriesAsVector(test_rle_data.rl_entries.data(),
            test_rle_data.rl_entries.data() + test_rle_data.rl_entries.size());

        // various preliminary structural checks
        ASSERT_EQ(test_scm_mask_payload.GetFirstVal(), test_rle_data.firstVal);
        //ASSERT_EQ(test_scm_mask_payload.GetMaskArray(), test_vector);
        ASSERT_EQ(test_scm_mask_payload.GetTransformID(), test_rle_data.transformID);
        ASSERT_EQ(test_scm_mask_payload.GetRlEntries(), rleEntriesAsVector);

        // Write scm mask paylaod into stringstream
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        test_scm_mask_payload.Write(bitwriter);

        ASSERT_TRUE(obj_payload.str().size() ==
                    test_scm_mask_payload.GetSize());

        // Read
        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader, static_cast < uint32_t>(test_vector.size())
        );

        ASSERT_EQ(test_scm_mask_payload.GetFirstVal(), recon_obj.GetFirstVal());
        ASSERT_EQ(test_scm_mask_payload.GetTransformID(),
                  recon_obj.GetTransformID());
        ASSERT_EQ(test_scm_mask_payload.GetRlEntries(),
                  recon_obj.GetRlEntries());

        // Decode the RLE in scm_mask_payload
        genie::contact::BinVecDtype recon_mask_array;
        decode_scm_mask_payload(recon_obj, test_vector.size(), recon_mask_array);

        ASSERT_TRUE(recon_mask_array == dummy_mask);
    }

    // ******************************************************************
    // Case: Intra SCM Single Tile
    // ******************************************************************

    std::vector<genie::core::record::ContactRecord> RECS;


    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood() && reader.peek() != EOF){
            RECS.emplace_back(bitreader);
        }
    }

    // One Tiles - Case 01
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);

        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        {
            std::string out_path = gitRootDir + "/tmp/encoded/contact/IntraSCM_Raw_SingleTile/";
            {
                std::ofstream tmp_writer(out_path + "case01-scm_payload.bin", std::ios::binary);
                genie::util::BitWriter tmp_bitwriter(&tmp_writer);
                scm_payload.Write(tmp_bitwriter);
            }
            {
                std::ofstream tmp_writer(out_path + "case01-scm_param.bin", std::ios::binary);
                genie::util::BitWriter tmp_bitwriter(&tmp_writer);
                scm_param.Write(tmp_bitwriter);
            }
            {
                std::ofstream tmp_writer(out_path + "case01-cm_param.bin", std::ios::binary);
                genie::core::Writer tmp_corewriter(&tmp_writer);
                cm_param.Write(tmp_corewriter);
            }
        }

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(scm_payload == recon_scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // One tiles - Case 02

    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // ******************************************************************
    // Case: Intra SCM Multi Tiles
    // ******************************************************************

    ASSERT_EQ(RECS.size(), 1);

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // ******************************************************************
    // Case: Inter SCM Single Tiles
    // ******************************************************************

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            genie::contact::UInt64VecDtype recon_start1 =
                genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            genie::contact::UInt64VecDtype recon_start2 =
                genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
//            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
            recon_end2 = genie::contact::sort(recon_end2);
            END2 = genie::contact::sort(END2);
            auto mask = genie::contact::equal(recon_end2, END2);
            ASSERT_TRUE(genie::contact::all(mask));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true;  // TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 1000u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            genie::contact::UInt64VecDtype recon_start1 =
                genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            genie::contact::UInt64VecDtype recon_start2 =
                genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // ******************************************************************
    // Case: Inter SCM Multi Tiles
    // ******************************************************************

    // Case 01
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            auto mask = genie::contact::not_equal(START1, recon_start1);
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }

    // Case 02
    {
        auto REMOVE_UNALIGNED_REGION = true;
        auto TRANSFORM_MASK = true;
        auto ENA_DIAG_TRANSFORM = true;
        auto ENA_BINARIZATION = true; //TODO(yeremia): enabling only binarization breaks the code!
        bool NORM_AS_WEIGHT = true;
        bool MULTIPLICATIVE_NORM = true;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto TILE_SIZE = 150u;
        auto MULT = 1u;

        auto cm_param = genie::contact::ContactMatrixParameters();
        auto scm_param = genie::contact::SubcontactMatrixParameters();
        auto scm_payload = genie::contact::SubcontactMatrixPayload();

        cm_param.SetBinSize(RECS.front().getBinSize());
        cm_param.SetTileSize(TILE_SIZE);

        for (auto& rec: RECS){
          cm_param.UpsertSample(rec.getSampleID(), rec.getSampleName());

          cm_param.UpsertChromosome(rec.getChr1ID(), rec.getChr1Name(),
                                    rec.getChr1Length());

          cm_param.UpsertChromosome(rec.getChr2ID(), rec.getChr2Name(),
                                    rec.getChr2Length());
        }

        auto& REC = RECS.front();
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::encode_scm(
            cm_param,
            rec,
            scm_param,
            scm_payload,
            REMOVE_UNALIGNED_REGION,
            TRANSFORM_MASK,
            ENA_DIAG_TRANSFORM,
            ENA_BINARIZATION,
            NORM_AS_WEIGHT,
            MULTIPLICATIVE_NORM,
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.Write(bitwriter);

        ASSERT_EQ(scm_payload.GetSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.GetNTilesInRow(), scm_param.GetNTilesInRow());
        ASSERT_EQ(scm_payload.GetNTilesInCol(), scm_param.GetNTilesInCol());
        ASSERT_EQ(scm_payload.GetSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(
            bitreader,
            cm_param,
            scm_param
        );

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(
            cm_param,
            scm_param,
            recon_scm_payload,
            recon_rec,
            MULT
        );

        ASSERT_EQ(recon_rec.getNumEntries(), REC.getNumEntries());
        {
            auto START1 = genie::contact::create_vector(REC.getStartPos1());
            auto recon_start1 = genie::contact::create_vector(recon_rec.getStartPos1());
            ASSERT_EQ(genie::contact::sort(recon_start1), genie::contact::sort(START1));
        }
        {
            auto END1 = genie::contact::create_vector(REC.getEndPos1());
            auto recon_end1 = genie::contact::create_vector(recon_rec.getEndPos1());
            ASSERT_EQ(genie::contact::sort(recon_end1), genie::contact::sort(END1));
        }
        {
            auto START2 = genie::contact::create_vector(REC.getStartPos2());
            auto recon_start2 = genie::contact::create_vector(recon_rec.getStartPos2());
            ASSERT_EQ(genie::contact::sort(recon_start2), genie::contact::sort(START2));
        }
        {
            auto END2 = genie::contact::create_vector(REC.getEndPos2());
            auto recon_end2 = genie::contact::create_vector(recon_rec.getEndPos2());
            ASSERT_EQ(genie::contact::sort(recon_end2), genie::contact::sort(END2));
        }
        {
            auto COUNT = genie::contact::create_vector(REC.getCounts());
            auto recon_count = genie::contact::create_vector(recon_rec.getCounts());
            ASSERT_EQ(genie::contact::sort(recon_count), genie::contact::sort(COUNT));
        }
    }
}
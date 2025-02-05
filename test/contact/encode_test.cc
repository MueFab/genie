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
#include <xtensor/xio.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"
//#include "genie/util/bitwriter.h"
//#include "genie/util/runtime-exception.h"
//#include <unistd.h>
#include <iostream>

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
            row_ids,
            col_ids,
            NENTRIES,
            NENTRIES,
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
            ROW_IDS,
            COL_IDS,
            NROWS,
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
            col_ids,
            NENTRIES,
            NENTRIES,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_mask, col_mask);

        genie::contact::remove_unaligned(
            row_ids,
            col_ids,
            IS_INTRA,
            row_mask,
            col_mask
        );

        ASSERT_EQ(row_ids(0), 0u) << "row_ids:" << row_ids << std::endl;
        ASSERT_EQ(row_ids(1), 2u) << "row_ids:" << row_ids << std::endl;
        ASSERT_EQ(col_ids(0), 1u) << "col_ids:" << col_ids << std::endl;
        ASSERT_EQ(col_ids(1), 3u) << "col_ids:" << col_ids << std::endl;

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

        ASSERT_EQ(row_ids(0), 0u) << "row_ids: " << row_ids << std::endl;
        ASSERT_EQ(row_ids(1), 1u) << "row_ids: " << row_ids << std::endl;
        ASSERT_EQ(col_ids(0), 0u) << "col_ids: " << col_ids << std::endl;
        ASSERT_EQ(col_ids(1), 1u) << "col_ids: " << col_ids << std::endl;

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

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});
        genie::contact::UIntVecDtype COUNTS = xt::adapt(COUNTS_VEC, {COUNTS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = ROW_IDS - ROW_ID_OFFSET;
        genie::contact::UInt64VecDtype col_ids = COL_IDS - COL_ID_OFFSET;

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
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
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids += ROW_ID_OFFSET;
        recon_col_ids += COL_ID_OFFSET;

        ASSERT_TRUE(xt::sort(ROW_IDS) == xt::sort(recon_row_ids)) << row_ids << recon_row_ids;
        ASSERT_TRUE(xt::sort(COL_IDS) == xt::sort(recon_col_ids)) << col_ids << recon_col_ids;
        ASSERT_TRUE(xt::sort(COUNTS) == xt::sort(recon_counts)) << COUNTS << recon_counts;
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

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});
        genie::contact::UIntVecDtype COUNTS = xt::adapt(COUNTS_VEC, {COUNTS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = ROW_IDS - ROW_ID_OFFSET;
        genie::contact::UInt64VecDtype col_ids = COL_IDS - COL_ID_OFFSET;

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
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
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids += ROW_ID_OFFSET;
        recon_col_ids += COL_ID_OFFSET;

        ASSERT_TRUE(xt::sort(ROW_IDS) == xt::sort(recon_row_ids)) << row_ids << recon_row_ids;
        ASSERT_TRUE(xt::sort(COL_IDS) == xt::sort(recon_col_ids)) << col_ids << recon_col_ids;
        ASSERT_TRUE(xt::sort(COUNTS) == xt::sort(recon_counts)) << COUNTS << recon_counts;
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

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});
        genie::contact::UIntVecDtype COUNTS = xt::adapt(COUNTS_VEC, {COUNTS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = ROW_IDS - ROW_ID_OFFSET;
        genie::contact::UInt64VecDtype col_ids = COL_IDS - COL_ID_OFFSET;

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
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
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids += ROW_ID_OFFSET;
        recon_col_ids += COL_ID_OFFSET;

        ASSERT_TRUE(xt::sort(ROW_IDS) == xt::sort(recon_row_ids)) << row_ids << recon_row_ids;
        ASSERT_TRUE(xt::sort(COL_IDS) == xt::sort(recon_col_ids)) << col_ids << recon_col_ids;
        ASSERT_TRUE(xt::sort(COUNTS) == xt::sort(recon_counts)) << COUNTS << recon_counts;
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

        genie::contact::UInt64VecDtype ROW_IDS = xt::adapt(ROW_IDS_VEC, {ROW_IDS_VEC.size()});
        genie::contact::UInt64VecDtype COL_IDS = xt::adapt(COL_IDS_VEC, {COL_IDS_VEC.size()});
        genie::contact::UIntVecDtype COUNTS = xt::adapt(COUNTS_VEC, {COUNTS_VEC.size()});

        genie::contact::UInt64VecDtype row_ids = ROW_IDS - ROW_ID_OFFSET;
        genie::contact::UInt64VecDtype col_ids = COL_IDS - COL_ID_OFFSET;

        genie::contact::UIntMatDtype tile_mat;
        genie::contact::sparse_to_dense(
            row_ids,
            col_ids,
            COUNTS,
            NROWS,
            NCOLS,
            tile_mat
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
            recon_row_ids,
            recon_col_ids,
            recon_counts
        );

        recon_row_ids += ROW_ID_OFFSET;
        recon_col_ids += COL_ID_OFFSET;

        ASSERT_TRUE(xt::sort(ROW_IDS) == xt::sort(recon_row_ids)) << row_ids << recon_row_ids;
        ASSERT_TRUE(xt::sort(COL_IDS) == xt::sort(recon_col_ids)) << col_ids << recon_col_ids;
        ASSERT_TRUE(xt::sort(COUNTS) == xt::sort(recon_counts)) << COUNTS << recon_counts;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): Create round trip test
TEST(ContactCoder, RoundTrip_Coding_DiagonalTransformation) {
    // None
    {
        auto MODE = genie::contact::DiagonalTransformMode::NONE;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 4},
                                                 {0, 2, 3},
                                                 {0, 0, 0}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }

    // Mode 0
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_0;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 4},
                                                 {0, 2, 3},
                                                 {0, 0, 0}};

        genie::contact::UIntMatDtype TARGET_MAT = {{1, 2, 0},
                                                   {0, 3, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_EQ(mat, TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 1 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 5},
                                                 {4, 0, 3},
                                                 {6, 0, 2}};

        genie::contact::UIntMatDtype TARGET_MAT = {{1, 0, 2},
                                                   {0, 3, 4},
                                                   {0, 5, 6}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_EQ(mat, TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 1 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 0, 4},
                                                 {3, 0, 2}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 0, 0},
                                                   {2, 3, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 1 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_1;
        genie::contact::UIntMatDtype ORIG_MAT = {{1, 2},
                                                 {3, 0},
                                                 {4, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 0},
                                                   {2, 3},
                                                   {0, 4}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }

    // Mode 2 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 6},
                                                 {2, 0, 5},
                                                 {1, 0, 4}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 2, 0},
                                                   {3, 0, 4},
                                                   {0, 5, 6}};

        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 2 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{2, 0, 4},
                                                 {1, 0, 3}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 2, 0},
                                                   {0, 3, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 2 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_2;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 4},
                                                 {2, 0},
                                                 {1, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 2},
                                                   {0, 3},
                                                   {0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }

    // Mode 3 Square
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 1},
                                                 {5, 0, 2},
                                                 {6, 0, 4}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 0, 2},
                                                   {3, 0, 4},
                                                   {5, 0, 6}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 3 nrows < ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{3, 0, 1},
                                                 {4, 0, 2}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 0, 2},
                                                   {3, 0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
    // Mode 3 nrows > ncols
    {
        auto MODE = genie::contact::DiagonalTransformMode::MODE_3;
        genie::contact::UIntMatDtype ORIG_MAT = {{2, 1},
                                                 {3, 0},
                                                 {4, 0}};
        genie::contact::UIntMatDtype TARGET_MAT = {{1, 2},
                                                   {0, 3},
                                                   {0, 4}};
        genie::contact::UIntMatDtype mat = genie::contact::UIntMatDtype(ORIG_MAT);

        genie::contact::diag_transform(mat, MODE);

        ASSERT_TRUE(mat == TARGET_MAT);

        genie::contact::inverse_diag_transform(mat, MODE);

        ASSERT_EQ(mat, ORIG_MAT);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_TransformRowBin) {
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
    genie::contact::transform_row_bin(orig_mat, bin_mat);

    ASSERT_EQ(bin_mat.shape(0), TARGET_BIN_MAT.shape(0));
    ASSERT_EQ(bin_mat.shape(1), TARGET_BIN_MAT.shape(1));
    ASSERT_EQ(bin_mat, TARGET_BIN_MAT);

    genie::contact::UIntMatDtype recon_mat;
    genie::contact::inverse_transform_row_bin(bin_mat, recon_mat);

    ASSERT_EQ(recon_mat.shape(0), MAT.shape(0));
    ASSERT_EQ(recon_mat.shape(1), MAT.shape(1));
    ASSERT_EQ(recon_mat, MAT);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_CodingCMTile) {
    // Test coding of bin_mat using JBIG
    {
        auto NROWS = 100u;
        auto NCOLS = 200u;
        auto CODEC_ID = genie::core::AlgoID::JBIG;

        genie::contact::BinMatDtype ORIG_BIN_MAT = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2u));

        auto bin_mat = genie::contact::BinMatDtype(ORIG_BIN_MAT);
        genie::contact::BinMatDtype recon_bin_mat;
        auto tile_payload = genie::contact::ContactMatrixTilePayload();

        encode_cm_tile(
            bin_mat,
            CODEC_ID,
            tile_payload
        );

        decode_cm_tile(
            tile_payload,
            CODEC_ID,
            recon_bin_mat
        );

        ASSERT_EQ(recon_bin_mat, ORIG_BIN_MAT);
    }
    // Test coding of bin_mat using JBIG
    {
        auto NROWS = 660u;
        auto NCOLS = 151u;
        auto CODEC_ID = genie::core::AlgoID::JBIG;

        genie::contact::BinMatDtype ORIG_BIN_MAT = xt::cast<bool>(xt::random::randint<uint16_t>({NROWS, NCOLS}, 0, 2u));

        auto bin_mat = genie::contact::BinMatDtype(ORIG_BIN_MAT);
        genie::contact::BinMatDtype recon_bin_mat;
        auto tile_payload = genie::contact::ContactMatrixTilePayload();

        encode_cm_tile(
            bin_mat,
            CODEC_ID,
            tile_payload
        );

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

        while (bitreader.IsStreamGood()) {
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
                cm_param.write(tmp_corewriter);
            }
        }

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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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


        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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


        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood()){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        LR_RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        while (bitreader.IsStreamGood()) {
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 =
                xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 =
                xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
//            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
            recon_end2 = xt::sort(recon_end2);
            END2 = xt::sort(END2);
            auto mask = xt::equal(recon_end2, END2);
            ASSERT_TRUE(xt::all(mask));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 =
                xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 =
                xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
        }
    }
}

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_SingleTiles_Downscale){

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood()){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        LR_RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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

            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_num_entries});
            genie::contact::UInt64VecDtype recon_row_ids = recon_start1 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_num_entries});
            genie::contact::UInt64VecDtype recon_col_ids = recon_start2 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype recon_counts = xt::adapt(recon_rec.getCounts(), {recon_num_entries});

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

            genie::contact::UInt64VecDtype lr_start1 = xt::adapt(LR_REC.getStartPos1(), {lr_num_entries});
            genie::contact::UInt64VecDtype lr_row_ids = recon_start1 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype lr_start2 = xt::adapt(LR_REC.getStartPos2(), {lr_num_entries});
            genie::contact::UInt64VecDtype lr_col_ids = recon_start2 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype lr_counts = xt::adapt(LR_REC.getCounts(), {lr_num_entries});

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

                genie::contact::UInt64VecDtype hr_start1 = xt::adapt(REC.getStartPos1(), {hr_num_entries});
                genie::contact::UInt64VecDtype hr_row_ids = recon_start1 / cm_param.getBinSize() / MULT;

                genie::contact::UInt64VecDtype hr_start2 = xt::adapt(REC.getStartPos2(), {hr_num_entries});
                genie::contact::UInt64VecDtype hr_col_ids = recon_start2 / cm_param.getBinSize() / MULT;

                genie::contact::UInt64VecDtype hr_counts = xt::adapt(REC.getCounts(), {hr_num_entries});

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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_MultTiles){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            auto mask = xt::not_equal(START1, recon_start1);
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.IsStreamGood()){
            LR_RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        LR_RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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

            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_num_entries});
            genie::contact::UInt64VecDtype recon_row_ids = recon_start1 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_num_entries});
            genie::contact::UInt64VecDtype recon_col_ids = recon_start2 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype recon_counts = xt::adapt(recon_rec.getCounts(), {recon_num_entries});

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

            genie::contact::UInt64VecDtype lr_start1 = xt::adapt(LR_REC.getStartPos1(), {lr_num_entries});
            genie::contact::UInt64VecDtype lr_row_ids = recon_start1 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype lr_start2 = xt::adapt(LR_REC.getStartPos2(), {lr_num_entries});
            genie::contact::UInt64VecDtype lr_col_ids = recon_start2 / cm_param.getBinSize() / MULT;

            genie::contact::UInt64VecDtype lr_counts = xt::adapt(LR_REC.getCounts(), {lr_num_entries});

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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);
        cm_param.upsertBinSizeMultiplier(MULT);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(LR_REC.getStartPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(LR_REC.getStartPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(LR_REC.getEndPos2(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(LR_REC.getCounts(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_RLESubcontactMatrixPayload) {
    {   // Case: Test if the RLE information is correct
        std::vector<bool> test_vector = {true, true, true, true, false, true, false, false};
        genie::contact::UIntVecDtype test_rl_entries = {4,1,1,2};
        genie::contact::BinVecDtype dummy_mask = xt::adapt(test_vector);
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
        genie::contact::UIntVecDtype test_rl_entries = {4,1,1,2};
        genie::contact::BinVecDtype dummy_mask = xt::adapt(test_vector);

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
        test_scm_mask_payload.setMaskArray(dummy_mask);
        test_scm_mask_payload.setRLEntries(test_rle_data.transformID, test_rle_data.firstVal, test_rle_data.rl_entries);

        // recast
        std::vector<uint32_t> rleEntriesAsVector(test_rle_data.rl_entries.data(),
            test_rle_data.rl_entries.data() + test_rle_data.rl_entries.size());

        // various preliminary structural checks
        ASSERT_EQ(test_scm_mask_payload.getFirstVal(), test_rle_data.firstVal);
        //ASSERT_EQ(test_scm_mask_payload.getMaskArray(), test_vector);
        ASSERT_EQ(test_scm_mask_payload.getTransformID(), test_rle_data.transformID);
        ASSERT_EQ(test_scm_mask_payload.getRLEntries(), rleEntriesAsVector);

        // Write scm mask paylaod into stringstream
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        test_scm_mask_payload.write(bitwriter);

        ASSERT_TRUE(obj_payload.str().size() == test_scm_mask_payload.getSize());

        // Read
        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader, static_cast < uint32_t>(test_vector.size())
        );

        ASSERT_EQ(test_scm_mask_payload.getFirstVal(), recon_obj.getFirstVal());
        ASSERT_EQ(test_scm_mask_payload.getTransformID(), recon_obj.getTransformID());
        ASSERT_EQ(test_scm_mask_payload.getRLEntries(), recon_obj.getRLEntries());

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

        while (bitreader.IsStreamGood()){
            RECS.emplace_back(bitreader);
        }
        RECS.pop_back();
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
                cm_param.write(tmp_corewriter);
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 =
                xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 =
                xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
//            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
            recon_end2 = xt::sort(recon_end2);
            END2 = xt::sort(END2);
            auto mask = xt::equal(recon_end2, END2);
            ASSERT_TRUE(xt::all(mask));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec : RECS) {
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 =
                xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 =
                xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            auto mask = xt::not_equal(START1, recon_start1);
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
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

        cm_param.setBinSize(RECS.front().getBinSize());
        cm_param.setTileSize(TILE_SIZE);

        for (auto& rec: RECS){
            cm_param.upsertSample(
                rec.getSampleID(),
                rec.getSampleName()
            );

            cm_param.upsertChromosome(
                rec.getChr1ID(),
                rec.getChr1Name(),
                rec.getChr1Length()
            );

            cm_param.upsertChromosome(
                rec.getChr2ID(),
                rec.getChr2Name(),
                rec.getChr2Length()
            );
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
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1));
        }
        {
            genie::contact::UInt64VecDtype START2 = xt::adapt(REC.getStartPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start2 = xt::adapt(recon_rec.getStartPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start2), xt::sort(START2));
        }
        {
            genie::contact::UInt64VecDtype END2 = xt::adapt(REC.getEndPos2(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end2 = xt::adapt(recon_rec.getEndPos2(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end2), xt::sort(END2));
        }
        {
            genie::contact::UInt64VecDtype COUNT = xt::adapt(REC.getCounts(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_count = xt::adapt(recon_rec.getCounts(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_count), xt::sort(COUNT));
        }
    }
}
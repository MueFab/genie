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
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include "helpers.h"
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"
#include "genie/util/bitreader.h"
//#include "genie/util/bitwriter.h"
//#include "genie/util/runtime-exception.h"

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
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.isGood()) {
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 9812u);
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), 9812u);
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
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.isGood()){
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 9812u);
        {
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1)) << xt::filter(START1, xt::not_equal(START1, recon_start1)) << xt::filter(recon_start1, xt::not_equal(START1, recon_start1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(REC.getEndPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1)) << xt::filter(END1, xt::not_equal(END1, recon_end1)) << xt::filter(recon_end1, xt::not_equal(END1, recon_end1));
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 9812u);
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
        std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-50000-21_21.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.isGood()){
            RECS.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        RECS.pop_back();
    }

    std::vector<genie::core::record::ContactRecord> LR_RECS;
    {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-250000-21_21.cont";
        std::string filepath = gitRootDir + "/data/records/contact/" + filename;

        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);


        while (bitreader.isGood()){
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1)) << xt::filter(START1, xt::not_equal(START1, recon_start1)) << xt::filter(recon_start1, xt::not_equal(START1, recon_start1));
        }
        {
            genie::contact::UInt64VecDtype END1 = xt::adapt(LR_REC.getEndPos1(), {LR_REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_end1 = xt::adapt(recon_rec.getEndPos1(), {recon_rec.getNumEntries()});
            ASSERT_EQ(xt::sort(recon_end1), xt::sort(END1)) << xt::filter(END1, xt::not_equal(END1, recon_end1)) << xt::filter(recon_end1, xt::not_equal(END1, recon_end1));
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 9812u);
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
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.isGood()) {
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 59740u);
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
            ASSERT_TRUE(xt::all(mask)) << xt::filter(recon_end2, !mask) << xt::filter(END2, !mask);
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
        genie::contact::encode_scm(cm_param, rec, scm_param, scm_payload, REMOVE_UNALIGNED_REGION, TRANSFORM_MASK,
                                   ENA_DIAG_TRANSFORM, ENA_BINARIZATION, CODEC_ID);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_scm_payload = genie::contact::SubcontactMatrixPayload(bitreader, cm_param, scm_param);

        ASSERT_TRUE(recon_scm_payload == scm_payload);

        auto recon_rec = genie::core::record::ContactRecord();

        decode_scm(cm_param, scm_param, recon_scm_payload, recon_rec, MULT);

        ASSERT_EQ(recon_rec.getNumEntries(), 59740u);
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

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Coding_InterSCM_Raw_MultTiles){
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> RECS;
    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.isGood()){
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 59740u);
        {
            genie::contact::UInt64VecDtype START1 = xt::adapt(REC.getStartPos1(), {REC.getNumEntries()});
            genie::contact::UInt64VecDtype recon_start1 = xt::adapt(recon_rec.getStartPos1(), {recon_rec.getNumEntries()});
            auto mask = xt::not_equal(START1, recon_start1);
            ASSERT_EQ(xt::sort(recon_start1), xt::sort(START1)) << xt::filter(START1, mask) << xt::filter(recon_start1, mask);
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
            CODEC_ID
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        scm_payload.write(bitwriter);

        ASSERT_EQ(scm_payload.getSampleID(), REC.getSampleID());
        ASSERT_EQ(scm_payload.getNTilesInRow(), scm_param.getNTilesInRow());
        ASSERT_EQ(scm_payload.getNTilesInCol(), scm_param.getNTilesInCol());
        ASSERT_EQ(scm_payload.getSize(), obj_payload.str().size());

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

        ASSERT_EQ(recon_rec.getNumEntries(), 59740u);
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
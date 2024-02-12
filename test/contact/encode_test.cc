/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <vector>
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include "genie/util/runtime-exception.h"
#include "genie/contact/contact_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, compute_mask) {

    std::vector<uint64_t> v = {0, 1, 3, 5};
    genie::contact::UInt64VecDtype ids = xt::adapt(v, {v.size()});
    genie::contact::BinVecDtype mask;

    genie::contact::UInt64VecDtype orig_ids = ids;

    genie::contact::compute_mask(ids, mask);

    auto shape = mask.shape(0);
    ASSERT_EQ(mask.size(), 6);
    ASSERT_EQ(xt::sum(xt::cast<uint8_t>(mask))(0), 4u);
    ASSERT_TRUE(mask(0));
    ASSERT_TRUE(mask(5));
    ASSERT_FALSE(mask(2));
    ASSERT_FALSE(mask(4));

    // ids may not changed
    ASSERT_TRUE(orig_ids == ids);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, compute_masks) {
    // Intra case
    {
        auto IS_INTRA = true;

        std::vector<uint64_t> row_ids_vec = {0, 2, 5};
        std::vector<uint64_t> col_ids_vec = {1, 4, 6};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(row_ids, col_ids, IS_INTRA, row_mask, col_mask);

        ASSERT_TRUE(row_mask == col_mask);
        ASSERT_EQ(row_mask.size(), 7u);
        ASSERT_EQ(col_mask.size(), 7u);

        ASSERT_TRUE(row_mask(0));
        ASSERT_TRUE(row_mask(1));
        ASSERT_TRUE(row_mask(2));
        ASSERT_FALSE(row_mask(3));
        ASSERT_TRUE(row_mask(4));
        ASSERT_TRUE(row_mask(5));
        ASSERT_TRUE(row_mask(6));
    }

    // Inter case
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> row_ids_vec = {0, 2, 5};
        std::vector<uint64_t> col_ids_vec = {1, 4, 6};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(row_ids, col_ids, IS_INTRA, row_mask, col_mask);

        ASSERT_TRUE(row_mask != col_mask);
        ASSERT_EQ(row_mask.size(), 6u);
        ASSERT_EQ(col_mask.size(), 7u);

        ASSERT_TRUE(row_mask(0));
        ASSERT_FALSE(row_mask(1));
        ASSERT_TRUE(row_mask(2));
        ASSERT_FALSE(row_mask(3));
        ASSERT_FALSE(row_mask(4));
        ASSERT_TRUE(row_mask(5));

        ASSERT_FALSE(col_mask(0));
        ASSERT_TRUE(col_mask(1));
        ASSERT_FALSE(col_mask(2));
        ASSERT_FALSE(col_mask(3));
        ASSERT_TRUE(col_mask(4));
        ASSERT_FALSE(col_mask(5));
        ASSERT_TRUE(col_mask(6));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, remove_unaligned) {
    // Intra case
    {
        auto IS_INTRA = true;

        std::vector<uint64_t> row_ids_vec = {0, 5};
        std::vector<uint64_t> col_ids_vec = {1, 6};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(row_ids, col_ids, IS_INTRA, row_mask, col_mask);
        genie::contact::remove_unaligned(row_ids, col_ids, IS_INTRA, row_mask, col_mask);

        ASSERT_EQ(row_ids(0), 0u);
        ASSERT_EQ(row_ids(1), 2u);
        ASSERT_EQ(col_ids(0), 1u);
        ASSERT_EQ(col_ids(1), 3u);
    }

    // Inter case
    {
        auto IS_INTRA = false;

        std::vector<uint64_t> row_ids_vec = {0, 5};
        std::vector<uint64_t> col_ids_vec = {1, 6};

        genie::contact::UInt64VecDtype row_ids = xt::adapt(row_ids_vec, {row_ids_vec.size()});
        genie::contact::UInt64VecDtype col_ids = xt::adapt(col_ids_vec, {col_ids_vec.size()});

        genie::contact::BinVecDtype row_mask;
        genie::contact::BinVecDtype col_mask;

        genie::contact::compute_masks(row_ids, col_ids, IS_INTRA, row_mask, col_mask);
        genie::contact::remove_unaligned(row_ids, col_ids, IS_INTRA, row_mask, col_mask);

        ASSERT_EQ(row_ids(0), 0u);
        ASSERT_EQ(row_ids(1), 1u);
        ASSERT_EQ(col_ids(0), 0u);
        ASSERT_EQ(col_ids(1), 1u);
    }
}

TEST(ContactCoder, sparse_to_dense) {
    // Test a matrix divided in 4 tiles
    // 5x5 scm matrix with tile size equals to 3
    {
        auto ROW_ID_OFFSET = 0;
        auto ROW_ID_OFFSET = 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
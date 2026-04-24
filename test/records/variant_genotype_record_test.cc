/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <vector>
#include "genie/core/record/variant/record.h"

namespace genie::core::record {

TEST(VariantGenotypeRecord, BasicProperties) {
    uint64_t variant_index = 1;
    uint32_t sample_index_from = 0;
    uint32_t sample_count = 2;

    std::vector<FormatField> format;

    std::vector<std::vector<int8_t>> alleles = {{0, 1}, {2, 3}};
    std::vector<std::vector<uint8_t>> phasing = {{4}, {5}}; // p-1 = 1
    std::vector<std::vector<uint32_t>> likelihoods = {{8, 9}, {10, 11}};

    std::optional<LinkRecord> link_record;

    VariantGenotype rec(
        variant_index, sample_index_from, sample_count, std::move(format),
        std::move(alleles), std::move(phasing), std::move(likelihoods), link_record);

    EXPECT_EQ(rec.GetVariantIndex(), variant_index);
    EXPECT_EQ(rec.GetSampleIndexFrom(), sample_index_from);
    EXPECT_EQ(rec.GetSampleCount(), sample_count);
    EXPECT_EQ(rec.GetFormatCount(), 0);
    EXPECT_EQ(rec.GetFormat().size(), 0);
    EXPECT_TRUE(rec.IsGenotypePresent());
    EXPECT_TRUE(rec.IsLikelihoodPresent());
    EXPECT_EQ(rec.GetNumberOfAllelesPerSample(), 2);
    EXPECT_FALSE(rec.GetLinkedRecord());
    EXPECT_EQ(rec.GetNumberOfLikelihoods(), 2);
}

} // namespace genie::core::record

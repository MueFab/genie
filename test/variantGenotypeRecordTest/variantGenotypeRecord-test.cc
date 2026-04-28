/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "genie/core/record/variant/record.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

class VariantGenotypeRecordTests : public ::testing::Test {
 protected:
    VariantGenotypeRecordTests() = default;
    ~VariantGenotypeRecordTests() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

TEST_F(VariantGenotypeRecordTests, RecordFilledWithOtherValues) {  // NOLINT(cert-err58-cpp)
    uint64_t variant_index = 1;
    uint32_t sample_index_from = 0;
    uint32_t sample_count = 2;
    std::vector<genie::core::record::FormatField> format;
    std::vector<std::vector<int8_t>> alleles = {{0, 1}, {2, 3}};
    std::vector<std::vector<uint8_t>> phasings = {{4, 5}, {6, 7}};
    std::vector<std::vector<uint32_t>> likelihoods = {{8, 9}, {10, 11}};
    std::optional<genie::core::record::LinkRecord> link_record;

    genie::core::record::VariantGenotype rec(
        variant_index, sample_index_from, sample_count,
        std::move(format),
        std::move(alleles),
        std::move(phasings),
        std::move(likelihoods),
        link_record);

    EXPECT_EQ(rec.GetVariantIndex(), variant_index);
    EXPECT_EQ(rec.GetSampleIndexFrom(), sample_index_from);
    EXPECT_EQ(rec.GetSampleCount(), sample_count);
    EXPECT_EQ(rec.GetFormat().size(), 0);
    EXPECT_EQ(rec.GetAlleles().size(), 2u);
    EXPECT_EQ(rec.GetPhasing().size(), 2u);
    EXPECT_EQ(rec.GetLikelihoods().size(), 2u);
    EXPECT_EQ(rec.GetLinkedRecord(), false);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST_F(VariantGenotypeRecordTests, RecordWithZeroValues) {  // NOLINT(cert-err58-cpp)
    uint64_t variant_index = 0;
    uint32_t sample_index_from = 0;
    uint32_t sample_count = 0;
    std::vector<genie::core::record::FormatField> format;
    std::vector<std::vector<int8_t>> alleles;
    std::vector<std::vector<uint8_t>> phasings;
    std::vector<std::vector<uint32_t>> likelihoods;
    std::optional<genie::core::record::LinkRecord> link_record;

    genie::core::record::VariantGenotype rec(
        variant_index, sample_index_from, sample_count,
        std::move(format),
        std::move(alleles),
        std::move(phasings),
        std::move(likelihoods),
        link_record);

    EXPECT_EQ(rec.GetVariantIndex(), 0);
    EXPECT_EQ(rec.GetSampleIndexFrom(), 0);
    EXPECT_EQ(rec.GetSampleCount(), 0);
    EXPECT_EQ(rec.GetFormat().size(), 0);
    EXPECT_EQ(rec.GetAlleles().size(), 0);
    EXPECT_EQ(rec.GetPhasing().size(), 0);
    EXPECT_EQ(rec.GetLikelihoods().size(), 0);
    EXPECT_EQ(rec.GetLinkedRecord(), false);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST_F(VariantGenotypeRecordTests, ReadFromFile) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/variant/1.3.5.header100.gt_only.vcf.geno";
    ASSERT_TRUE(std::filesystem::exists(filepath)) << filepath;

    std::vector<genie::core::record::VariantGenotype> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()) {
            recs.emplace_back(bitreader);
        }
        recs.pop_back();
    }

    ASSERT_EQ(recs.size(), 100);

    auto& rec = recs[0];
    EXPECT_EQ(rec.GetVariantIndex(), 0);
    EXPECT_EQ(rec.GetSampleCount(), 1092u);
    EXPECT_EQ(rec.GetNumberOfAllelesPerSample(), 2);
    EXPECT_EQ(rec.GetLinkedRecord(), false);
    EXPECT_EQ(rec.GetAlleles().size(), 1092u);
}

// ---------------------------------------------------------------------------------------------------------------------
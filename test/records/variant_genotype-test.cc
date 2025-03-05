/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bit_reader.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(VariantGenotypeRecord, Genotype_gt_only) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::vector<genie::core::record::VariantGenotype> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    ASSERT_EQ(recs.size(), 100);

    for (size_t i = 0; i< recs.size(); i++){
        auto& rec = recs[i];
        ASSERT_EQ(rec.GetVariantIndex(), i);
        ASSERT_EQ(rec.GetNumSamples(), 1092u);
        ASSERT_EQ(rec.GetNumberOfAllelesPerSample(), 2);
        ASSERT_EQ(rec.GetLinkedRecord(), false);
    }

    {
        auto& rec = recs[0];
        auto num_samples = rec.GetNumSamples();
        auto num_alleles = rec.GetNumberOfAllelesPerSample();

        std::vector<uint32_t> num_counts_per_allele(3);
        auto& rec_alleles = rec.GetAlleles();
        for (size_t i = 0; i < num_samples; i++){
            for (size_t j = 0; j < num_alleles; j++){
                auto v = rec_alleles[i][j];
                num_counts_per_allele[(size_t)v] += 1;
            }
        }

        ASSERT_EQ(num_counts_per_allele[0], 2067u);
        ASSERT_EQ(num_counts_per_allele[1], 117u);
        ASSERT_EQ(num_counts_per_allele[2], 0u);
    }

    {
        auto& rec = recs[recs.size()-1];
        auto num_samples = rec.GetNumSamples();
        auto num_alleles = rec.GetNumberOfAllelesPerSample();

        std::vector<uint32_t> num_counts_per_allele(3);
        auto& rec_alleles = rec.GetAlleles();
        for (size_t i = 0; i < num_samples; i++){
            for (size_t j = 0; j < num_alleles; j++){
                auto v = rec_alleles[i][j];
                num_counts_per_allele[(size_t)v] += 1;
            }
        }

        ASSERT_EQ(num_counts_per_allele[0], 2178u);
        ASSERT_EQ(num_counts_per_allele[1], 6u);
        ASSERT_EQ(num_counts_per_allele[2], 0u);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(VariantGenotypeRecord, Genotype_FORMAT_only) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.fmt_only.vcf.geno";

    std::vector<genie::core::record::VariantGenotype> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    ASSERT_EQ(recs.size(), 100);

    for (size_t i = 0; i < recs.size(); i++) {
        auto& rec = recs[i];
        ASSERT_EQ(rec.GetVariantIndex(), i);
        ASSERT_EQ(rec.GetNumSamples(), 1092u);
        ASSERT_EQ(rec.GetLinkedRecord(), false);
    }
    
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(VariantGenotypeRecord, Genotype_FORMAT_CASE2) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/variant/ALL.chrY.phase3_integrated_v2a.20130502.genotypes.bgz.CASE02.geno";

    std::vector<genie::core::record::VariantGenotype> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    ASSERT_EQ(recs.size(), 5);

    for (size_t i = 0; i < recs.size(); i++) {
        auto& rec = recs[i];
        ASSERT_EQ(rec.GetVariantIndex(), i);
        ASSERT_EQ(rec.GetNumSamples(), 1233u);
        ASSERT_EQ(rec.GetLinkedRecord(), false);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

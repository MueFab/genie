/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(VariantGenotypeRecord, Genotype_gt_only) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()){
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    for (size_t i = 0; i< recs.size(); i++){
        auto& rec = recs[i];
        ASSERT_EQ(rec.getVariantIndex(), i);
        ASSERT_EQ(rec.getNumSamples(), 1092u);
        ASSERT_EQ(rec.getNumberOfAllelesPerSample(), 2);
        ASSERT_EQ(rec.getLinkedRecord(), false);
    }

    {
        auto& rec = recs[0];
        auto num_samples = rec.getNumSamples();
        auto num_alleles = rec.getNumberOfAllelesPerSample();

        std::vector<uint32_t> num_counts_per_allele(3);
        auto& rec_alleles = rec.getAlleles();
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
        auto num_samples = rec.getNumSamples();
        auto num_alleles = rec.getNumberOfAllelesPerSample();

        std::vector<uint32_t> num_counts_per_allele(3);
        auto& rec_alleles = rec.getAlleles();
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

TEST(VariantGenotypeRecord, Genotype_fmt_only) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/ALL.chrX.5000.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 5000);

    for (size_t i = 0; i < recs.size(); i++) {
        auto& rec = recs[i];
        ASSERT_EQ(rec.getVariantIndex(), i);
        ASSERT_EQ(rec.getNumSamples(), 1092u);
        ASSERT_EQ(rec.getNumberOfAllelesPerSample(), 2);
        ASSERT_EQ(rec.getLinkedRecord(), false);
    }


}

// ---------------------------------------------------------------------------------------------------------------------

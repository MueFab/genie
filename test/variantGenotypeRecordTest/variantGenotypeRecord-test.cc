#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "genie/core/record/variant_genotype/arrayType.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bitreader.h"

class RandomGenotype {
 public:
    genie::core::record::variant_genotype::Record randomVariantGenotype() {
        uint64_t variant_index = 0;
        uint32_t sample_index_from = 0;
        uint32_t sample_count = 0;
        uint8_t format_count = 0;
        std::vector<genie::core::record::variant_genotype::FormatField> format{};
        uint8_t genotype_present = 0;
        uint8_t likelihood_present = 0;
        uint8_t n_alleles_per_sample = 0;
        std::vector<std::vector<uint8_t>> alleles{};
        std::vector<std::vector<uint8_t>> phasing{};
        uint8_t n_likelihoods = 0;
        std::vector<std::vector<uint32_t>> likelihoods{};
        uint8_t linked_record = 0;
        uint8_t link_name_len = 0;
        std::string link_name = "";
        uint8_t reference_box_ID = 0;

        return genie::core::record::variant_genotype::Record(
            variant_index, sample_index_from, sample_count, format_count, format, genotype_present, likelihood_present,
            n_alleles_per_sample, alleles, phasing, n_likelihoods, likelihoods, linked_record, link_name_len, link_name,
            reference_box_ID);
    }
};

class VariantGenotypeRecordTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    VariantGenotypeRecordTests() = default;

    ~VariantGenotypeRecordTests() override = default;

    // Use SetUp instead of the constructor in the following cases:
    // - In the body of a constructor (or destructor), it's not possible to
    //   use the ASSERT_xx macros. Therefore, if the set-up operation could
    //   cause a fatal test failure that should prevent the test from running,
    //   it's necessary to use a CHECK macro or to use SetUp() instead of a
    //   constructor.
    // - If the tear-down operation could throw an exception, you must use
    //   TearDown() as opposed to the destructor, as throwing in a destructor
    //   leads to undefined behavior and usually will kill your program right
    //   away. Note that many standard libraries (like STL) may throw when
    //   exceptions are enabled in the compiler. Therefore you should prefer
    //   TearDown() if you want to write portable tests that work with or
    //   without exceptions.
    // - The googletest team is considering making the assertion macros throw
    //   on platforms where exceptions are enabled (e.g. Windows, Mac OS, and
    //   Linux client-side), which will eliminate the need for the user to
    //   propagate failures from a subroutine to its caller. Therefore, you
    //   shouldn't use googletest assertions in a destructor if your code
    //   could run on such a platform.
    // - In a constructor or destructor, you cannot make a virtual function
    //   call on this object. (You can call a method declared as virtual, but
    //   it will be statically bound.) Therefore, if you need to call a method
    //   that will be overridden in a derived class, you have to use
    //   SetUp()/TearDown().

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(VariantGenotypeRecordTests, DISABLED_readFilefrombin) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.10000";
    std::ifstream inputfile;
    inputfile.open(path + filename + ".geno", std::ios::in | std::ios::binary);
    if (inputfile.is_open()) {
        genie::util::BitReader reader(inputfile);
        std::ofstream outputfile(path + filename + "_geno.txt");
        do {
            genie::core::record::variant_genotype::Record variant_geno_record(reader);
            EXPECT_EQ(variant_geno_record.getSampleCount(), variant_geno_record.getAlleles().size());
            variant_geno_record.write(outputfile);
        } while (inputfile.peek() != EOF);
        inputfile.close();
        outputfile.close();
    }
}

TEST_F(VariantGenotypeRecordTests, recordFilledWithZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::variant_genotype::Record variant_geno_record;

    EXPECT_EQ(variant_geno_record.getVariantIndex(), 0);
    EXPECT_EQ(variant_geno_record.getFormat().size(), 0);
    EXPECT_EQ(variant_geno_record.getStartSampleIndex(), (uint32_t)0);
    EXPECT_EQ(variant_geno_record.getSampleCount(), (uint32_t)0);
    EXPECT_EQ(variant_geno_record.getFormatCount(), (uint8_t)0);
    EXPECT_EQ(variant_geno_record.getFormat().size(), 0);
    EXPECT_FALSE(variant_geno_record.isGenotypePresent());
    EXPECT_FALSE(variant_geno_record.isLikelihoodPresent());
    EXPECT_EQ(variant_geno_record.getNumberOfAllelesPerSample(), (uint8_t)1);
    EXPECT_EQ(variant_geno_record.getAlleles().size(), 0);
    EXPECT_EQ(variant_geno_record.getPhasing().size(), 0);
    EXPECT_EQ(variant_geno_record.getNumberOfLikelihoods(), (uint8_t)0);
    EXPECT_EQ(variant_geno_record.getLikelihoods().size(), 0);
    EXPECT_FALSE(variant_geno_record.isLinkedRecord());
    EXPECT_EQ(variant_geno_record.getLinkName().size(), 0);
    EXPECT_EQ(variant_geno_record.getReferenceBoxID(), (uint8_t)0);
}

TEST_F(VariantGenotypeRecordTests, recordFilledWithOtherValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint64_t variant_index = 1;
    uint32_t sample_index_from = 0;
    uint32_t sample_count = 2;

    uint8_t format_count = 0;
    std::vector<genie::core::record::variant_genotype::FormatField> format;

    uint8_t genotype_present = 1;
    uint8_t likelihood_present = 1;

    uint8_t n_alleles_per_sample = 1;
    std::vector<std::vector<uint8_t>> alleles = {{0, 1}, {2, 3}};
    std::vector<std::vector<uint8_t>> phasing = {{4, 5}, {6, 7}};

    uint8_t n_likelihoods = 2;
    std::vector<std::vector<uint32_t>> likelihoods = {{8, 9}, {10, 11}};

    uint8_t linked_record = 0;

    genie::core::record::variant_genotype::Record variant_geno_record(
        variant_index, sample_index_from, sample_count, format_count, format, genotype_present, likelihood_present,
        n_alleles_per_sample, alleles, phasing, n_likelihoods, likelihoods, linked_record);

    EXPECT_EQ(variant_geno_record.getVariantIndex(), variant_index);
    EXPECT_EQ(variant_geno_record.getStartSampleIndex(), sample_index_from);
    EXPECT_EQ(variant_geno_record.getSampleCount(), sample_count);
    EXPECT_EQ(variant_geno_record.getFormatCount(), format_count);
    EXPECT_EQ(variant_geno_record.getFormat().size(), 0);
    EXPECT_EQ(variant_geno_record.isGenotypePresent(), genotype_present);
    EXPECT_EQ(variant_geno_record.isLikelihoodPresent(), likelihood_present);
    EXPECT_EQ(variant_geno_record.getNumberOfAllelesPerSample(), n_alleles_per_sample);
    EXPECT_EQ(variant_geno_record.isLinkedRecord(), linked_record);
    EXPECT_EQ(variant_geno_record.getAlleles(), alleles);
    EXPECT_EQ(variant_geno_record.getPhasing(), phasing);
    EXPECT_EQ(variant_geno_record.getLikelihoods(), likelihoods);
    EXPECT_EQ(variant_geno_record.getNumberOfLikelihoods(), n_likelihoods);
}
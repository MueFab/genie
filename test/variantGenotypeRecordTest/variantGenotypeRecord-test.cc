#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bitreader.h"

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

#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
TEST_F(VariantGenotypeRecordTests, readFilefrombin) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    std::string gitRootDir = exec("git rev-parse --show-toplevel");

    std::filebuf fb1;
    if (fb1.open(gitRootDir.substr(0, gitRootDir.length() - 1) + "/data/records/1.3.05_cut.geno",
                 std::ios::in | std::ios::binary)) {
        std::istream is(&fb1);
        genie::util::BitReader reader(is);
        std::ofstream myfile("1.3.05_cut.geno_test.txt");
        do {
            genie::core::record::variant_genotype::Record variant_geno_record(reader);

            EXPECT_EQ(variant_geno_record.getSampleCount(), variant_geno_record.getAlleles().size());

            variant_geno_record.write(myfile);

        } while (is.peek() != EOF);
        fb1.close();
        myfile.close();
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
#include <gtest/gtest.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "genie/annotation/annotation.h"
#include "helpers.h"

class AnnotationTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AnnotationTests() = default;

    ~AnnotationTests() override = default;

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

// compressor compressorID stepID algorithmID {parameter1 parameter2 ... parametern} {invar1 invar2 ... invarn} {outvar1
// outvar2 ... outvarn}
TEST_F(AnnotationTests, compressorConfig) {
    genie::annotation::Compressor compressors;
    std::string comment = "# 23fnv0-33 bla";
    std::string set1 = "compressor 1 1 BSC";
    std::string set2 = "compressor 2 2 LZMA";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n';
    compressors.parseConfig(config);
    EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}

TEST_F(AnnotationTests, compressorConfigSteps) {
    genie::annotation::Compressor compressors;
    std::string comment = "# with parameters";
    std::string set1 = "compressor 1 1 BSC {32 128 1 1}";
    std::string set2 = "compressor 2 2 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 2 1 ZSTD {0 0}";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n' << set3 << '\n';
    compressors.parseConfig(config);
    EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}
TEST_F(AnnotationTests, compressorConfigcompressors) {
    genie::annotation::Compressor compressors;
    std::string comment = "# with parameters";
    std::string set1 = "compressor 1 1 BSC {32 128 1 1}";
    std::string set2 = "compressor 3 2 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 1 2 ZSTD {0 0}";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n' << set3 << '\n';
    compressors.parseConfig(config);
    EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}

TEST_F(AnnotationTests, annotationSite) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filePath = gitRootDir + "/data/records/";
    std::string inputFilename = filePath + "ALL.chrX.10000.site";
    std::string outputFilename = filePath + "ALL.chrX.10000_site_annotation";

    std::filesystem::remove(outputFilename + ".bin");

    genie::annotation::Annotation annotationGenerator;
    std::string set1 = "compressor 1 0 BSC {32 128 1 1}";
    std::string set2 = "compressor 1 1 ZSTD";// {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 2 0 LZMA {8}";
    std::string set4 = "compressor 3 0 BSC {16 128 1 1}";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n' << set3 << '\n' << set4 << '\n';


    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.startStream(genie::annotation::RecType::SITE_FILE, inputFilename, outputFilename);

    EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin"));
    auto filesize = std::filesystem::file_size(outputFilename + ".bin");
    size_t expectedSize = 200 * 1024;  // at least 200kB
    EXPECT_LE(expectedSize, filesize);
}
TEST_F(AnnotationTests, annotationGeno) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filePath = gitRootDir + "/data/records/";
    std::string inputFilename = filePath + "1.3.5.header100.gt_only.vcf.geno";
    std::string outputFilename = filePath + "1.3.5.header100.gt_only.vcf_geno_annotation";

    std::filesystem::remove(outputFilename + ".bin");

    std::string set1 = "compressor 1 0 BSC";
    std::string set2 = "compressor 1 1 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' <<  set3 << '\n' << set4 << '\n';


    genie::annotation::Annotation annotationGenerator;
    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.startStream(genie::annotation::RecType::GENO_FILE, inputFilename, outputFilename);

    EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin"));

    auto filesize = std::filesystem::file_size(outputFilename + ".bin");
    size_t expectedSize = 4 * 1024;  // at least 4kB
    EXPECT_LE(expectedSize, filesize);
}
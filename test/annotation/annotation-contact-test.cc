/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */


#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "genie/annotation/annotation.h"
#include "helpers.h"
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"


class AnnotationContactTests : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    AnnotationContactTests() = default;

    ~AnnotationContactTests() override = default;

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


TEST_F(AnnotationContactTests, contactparameterset) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filePath = gitRootDir + "/data/records/contact/";
    std::string inputFilename = filePath +  "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";


    ASSERT_TRUE(std::filesystem::exists(inputFilename));

    std::string outputFilename = inputFilename + "-output";
    std::filesystem::remove(outputFilename+ ".bin");


    genie::annotation::Compressor compressors;
    std::string comment = "# with parameters";
    std::string set1 = "compressor 1 1 BSC {32 128 1 1}";
    std::string set2 = "compressor 3 2 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 1 2 ZSTD {0 0}";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n' << set3 << '\n';

    genie::annotation::ContactMatrixParameters cmParameters;

    genie::annotation::Annotation annotationGenerator;
    annotationGenerator.setTileSize(100, 3000);
    annotationGenerator.setContactOptions(cmParameters);
    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.startStream(genie::annotation::RecType::CM_FILE, inputFilename, outputFilename);
    EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin")) << outputFilename + ".bin";
}

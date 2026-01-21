/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include "genie/annotation/annotation.h"

#include "helpers.h"

class GeneExpressionConformanceTest : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    GeneExpressionConformanceTest() = default;

    ~GeneExpressionConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(GeneExpressionConformanceTest, GeneExpressionConformanceTests) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename;

    uint32_t BLOCK_SIZE = 200;

    std::string set1 = "compressor 1 0 BSC {32 128 1 1}";
    std::string set2 = "compressor 1 1 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Annotation annotationGenerator;
    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.setTileSize(BLOCK_SIZE, 3000);

    annotationGenerator.startStream(genie::annotation::RecType::GENE_EXPRESSION, filepath, filepath + "_output");
}

INSTANTIATE_TEST_SUITE_P(
    testallGeneExpressionConformance, GeneExpressionConformanceTest,
    ::testing::Values(
        "/data/records/expression/5.4.1-tp01.exp"
    ));
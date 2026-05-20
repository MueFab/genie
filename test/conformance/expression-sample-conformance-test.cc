/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "genie/annotation/annotation.h"
#include "helpers.h"

class ExpressionSampleConformanceTest : public ::testing::TestWithParam<std::string> {
 protected:
    ExpressionSampleConformanceTest() = default;

    ~ExpressionSampleConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(ExpressionSampleConformanceTest, ExpressionSampleConformancetests) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename;

    std::string set1 = "compressor 1 0 SER {0} {} {1}";
    std::string set2 = "compressor 1 1 BSC {} {{0 0 0}} {0}";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set2 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Annotation annotationGenerator;

    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.setTileSize(1000, 3000);
    annotationGenerator.setATType(
        genie::core::record::annotation_access_unit::AnnotationType::GENE_EXPRESSION,
        genie::core::record::annotation_access_unit::AnnotationSubtype::GENE_EXPRESSION);

    annotationGenerator.startStream(genie::annotation::RecType::SAMPLE_FILE, filepath, filepath + "_output");
}

INSTANTIATE_TEST_SUITE_P(testallExpressionSampleConformance, ExpressionSampleConformanceTest,
    ::testing::Values("/data/records/expression/5.4.1-tp01.sample"));

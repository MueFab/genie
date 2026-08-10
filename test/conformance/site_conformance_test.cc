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

class SiteConformanceTest : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    SiteConformanceTest() = default;

    ~SiteConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(SiteConformanceTest, SiteConformancetests) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename;

    std::string set1 = "compressor 1 0 BSC";
    std::string set2 = "compressor 1 1 LZMA";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Annotation annotationGenerator;

    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.setTileSize(1000, 3000);

    annotationGenerator.startStream(genie::annotation::RecType::SITE_FILE, filepath, filepath + "_output");
}

INSTANTIATE_TEST_SUITE_P(testallsiteConformance, SiteConformanceTest,
    ::testing::Values("/data/records/conformance/1.3.5.bgz.CASE01.site",
                      "/data/records/conformance/1.3.11.bgz.CASE03.site",
                      "/data/records/conformance/1.3.11.bgz.CASE04.site"));

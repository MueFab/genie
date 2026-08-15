/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "genie/core/access_unit/annotation/record.h"

#include "genie/annotation/annotation.h"
#include "helpers.h"

using AnnotationSubtype = genie::core::access_unit::annotation::AnnotationSubtype;
using TestParam = std::pair<std::string, AnnotationSubtype>;

class TrackPropertyConformanceTest : public ::testing::TestWithParam<TestParam> {
 protected:
    // Do any necessary setup for your tests here
    TrackPropertyConformanceTest() = default;

    ~TrackPropertyConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(TrackPropertyConformanceTest, TrackPropertyConformancetests) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    const auto& [filename, subtype] = GetParam();
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

    annotationGenerator.startStream(genie::annotation::RecType::TRACK_PROPERTY_FILE, filepath, filepath + "_output");
}

INSTANTIATE_TEST_SUITE_P(testallTrackPropertyConformance, TrackPropertyConformanceTest,
    ::testing::Values(
        TestParam{"/data/records/track/TD01_snATAC_-_Endo_Lympho_prop.mgrec", AnnotationSubtype::WIG},
        TestParam{"/data/records/track/TD02_concat_KGS_Hg19_Chr1_50_prop.mgrec", AnnotationSubtype::BED}/*
        TestParam{"/data/records/track/Kidney_Genetic_Scorecard_WIG_Hg19_Chr1_prop.mgrec", AnnotationSubtype::WIG},
        TestParam{"/data/records/track/Kidney_Genetic_Scorecard_INT_Hg19_Chr1_prop.mgrec", AnnotationSubtype::BED},
        TestParam{"/data/records/track/concat_BED_prop.mgrec", AnnotationSubtype::BED},
        TestParam{"/data/records/track/concat_INT_prop.mgrec", AnnotationSubtype::BED},
        TestParam{"/data/records/track/concat_WIG_prop.mgrec", AnnotationSubtype::WIG}*/));

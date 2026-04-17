/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "codecs/include/mpegg-codecs.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/data_unit_record/record.h"
#include "genie/core/track_record/record.h"
#include "genie/track/track_parser.h"

#include "genie/annotation/annotation.h"
#include "helpers.h"

using AnnotationSubtype = genie::core::record::annotation_access_unit::AnnotationSubtype;
using TestParam = std::pair<std::string, AnnotationSubtype>;

class TrackDataConformanceTest : public ::testing::TestWithParam<TestParam> {
 protected:
    // Do any necessary setup for your tests here
    TrackDataConformanceTest() = default;

    ~TrackDataConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(TrackDataConformanceTest, TrackDataConformancetests) {  // NOLINT(cert-err58-cpp)
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
    annotationGenerator.setATType(genie::core::record::annotation_access_unit::AnnotationType::TRACKS,
                                   subtype);

    annotationGenerator.startStream(genie::annotation::RecType::TRACK_FILE, filepath, filepath + "_output");
}

// Test case details will be added when example files become available
INSTANTIATE_TEST_SUITE_P(testallTrackDataConformance, TrackDataConformanceTest,
                        ::testing::Values(
                            TestParam{"/data/records/track/trfMask_bed.mgrec", AnnotationSubtype::BED},
                            TestParam{"/data/records/track/Kidney_Genetic_Scorecard_BED_Hg19_Chr1_data.mgrec", AnnotationSubtype::BED},
                            TestParam{"/data/records/track/Kidney_Genetic_Scorecard_WIG_Hg19_Chr1_data.mgrec", AnnotationSubtype::WIG}));

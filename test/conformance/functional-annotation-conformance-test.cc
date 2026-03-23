/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include <codecs/include/mpegg-codecs.h>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/parameterset_composer.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/data_unit_record/record.h"
#include "genie/core/functional_annotation_record/record.h"
#include "genie/functionalannotation/functionalannotation_parser.h"

#include "genie/annotation/annotation.h"
#include "helpers.h"

class FunctionalAnnotationConformanceTest : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    FunctionalAnnotationConformanceTest() = default;

    ~FunctionalAnnotationConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(FunctionalAnnotationConformanceTest, FunctionalAnnotationConformancetests) {  // NOLINT(cert-err58-cpp)
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

    annotationGenerator.startStream(genie::annotation::RecType::FUNCTIONAL_ANNOTATIONS_FILE, filepath, filepath + "_output");
}

// Test case details will be added when example files become available
INSTANTIATE_TEST_SUITE_P(testallFunctionalAnnotationConformance, FunctionalAnnotationConformanceTest,
                        ::testing::Values());

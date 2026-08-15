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

class FunctionalAnnotationConformanceTest : public ::testing::TestWithParam<TestParam> {
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
    annotationGenerator.setATType(genie::core::access_unit::annotation::AnnotationType::FUNCTIONAL_ANNOTATIONS,
                                   subtype);

    annotationGenerator.startStream(genie::annotation::RecType::FUNCTIONAL_ANNOTATION_FILE, filepath, filepath + "_output");
}

INSTANTIATE_TEST_SUITE_P(
    testallFunctionalAnnotationConformance, FunctionalAnnotationConformanceTest,
    ::testing::Values(
        TestParam{"/data/records/functional_annotation/FA01_Homo_sapiens.GRCh38.95.chr.gtf.mgrec", AnnotationSubtype::GTF},
        TestParam{"/data/records/functional_annotation/FA02_Homo_sapiens.GRCh38.95.chr.gtf.mgrec", AnnotationSubtype::GTF},
        TestParam{"/data/records/functional_annotation/FA03_Homo_sapiens.GRCh38.95.gff3.mgrec", AnnotationSubtype::GFF},
        TestParam{"/data/records/functional_annotation/FA04_Homo_sapiens.GRCh38.95.gff3.mgrec", AnnotationSubtype::GFF}/*,
        TestParam{"/data/records/functional_annotation/Homo_sapiens.GRCh38.95.chr.gff3.mgrec", AnnotationSubtype::GFF},
        TestParam{"/data/records/functional_annotation/Homo_sapiens.GRCh38.95.chr.gtf.mgrec", AnnotationSubtype::GTF},
        TestParam{"/data/records/functional_annotation/Homo_sapiens.GRCh38.95.gff3.mgrec", AnnotationSubtype::GFF}*/));

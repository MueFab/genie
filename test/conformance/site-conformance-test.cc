#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include <codecs/include/mpegg-codecs.h>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/annotation/annotation.h"
#include "genie/variantsite/ParameterSetComposer.h"
#include "genie/variantsite/VariantSiteParser.h"
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
    std::string infofilename = "/data/records/conformance/1.3.11.bgz.infotags.json";


    std::string set1 = "compressor 1 0 BSC";
    std::string set2 = "compressor 1 1 LZMA";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Annotation annotationGenerator;

    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.startStream(genie::annotation::RecType::SITE_FILE, filepath, gitRootDir + infofilename,
                                    filepath + "_output");
}

INSTANTIATE_TEST_CASE_P(testallsiteConformance, SiteConformanceTest,
                        ::testing::Values("/data/records/conformance/1.3.5.bgz.CASE01.site",
                                          "/data/records/conformance/1.3.11.bgz.CASE03.site",
                                          "/data/records/conformance/1.3.11.bgz.CASE04.site"));
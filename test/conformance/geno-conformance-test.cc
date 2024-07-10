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
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/annotation/annotation.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

#include "helpers.h"

struct genoTestValues {
    genie::genotype::SortingAlgoID SortingID;
    genie::genotype::BinarizationID binID;
    genie::core::AlgoID algID;
    std::string filepath;
    genoTestValues(genie::genotype::SortingAlgoID _SortingID, genie::genotype::BinarizationID _binID,
                   genie::core::AlgoID _algID, std::string _filepath)
        : SortingID(_SortingID), binID(_binID), algID(_algID), filepath(_filepath) {}
};

class GenotypeConformanceTest : public ::testing::TestWithParam<genoTestValues> {
 protected:
    // Do any necessary setup for your tests here
    GenotypeConformanceTest() = default;

    ~GenotypeConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

#define WRITETESTFILES

TEST_P(GenotypeConformanceTest, GenoConformanceTests) {
    genoTestValues testparams = GetParam();
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    std::string name = gitRootDir + testparams.filepath + ".geno";
    ASSERT_TRUE(std::filesystem::exists(name)) << name << "\n ";

    if (testparams.algID == genie::core::AlgoID::JBIG) name += "_JBIG";
    if (testparams.algID == genie::core::AlgoID::BSC) name += "_BSC";
    if (testparams.binID == genie::genotype::BinarizationID::BIT_PLANE) name += "_BITPLANE";
    if (testparams.binID == genie::genotype::BinarizationID::ROW_BIN) name += "_ROWBIN";
    if (testparams.SortingID == genie::genotype::SortingAlgoID::NO_SORTING) name += "_NOSORT";
    if (testparams.SortingID == genie::genotype::SortingAlgoID::RANDOM_SORT) name += "_RANDOMSORT";

    uint32_t BLOCK_SIZE = 200;
    bool TRANSFORM_MODE = true;

    genie::likelihood::EncodingOptions likelihood_opt = {
        BLOCK_SIZE,      // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    genie::genotype::EncodingOptions genotype_opt = {
        BLOCK_SIZE,                                  // block_size;
        testparams.binID,                            // genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        testparams.SortingID,                        // genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        testparams.SortingID,                        // genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        testparams.algID                             // codec_ID;
    };

    std::string set1 = "compressor 1 0 BSC {32 128 1 1}";
    std::string set2 = "compressor 1 1 LZMA {8 16777216 3 0 2 32}";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Annotation annotationGenerator;
    annotationGenerator.setCompressorConfig(config);
    annotationGenerator.setGenotypeOptions(genotype_opt);
    annotationGenerator.setLikelihoodOptions(likelihood_opt);
    annotationGenerator.startStream(genie::annotation::RecType::GENO_FILE, gitRootDir + testparams.filepath + ".geno",
                                    name);
}

INSTANTIATE_TEST_CASE_P(
    testallGenoConformance, GenotypeConformanceTest,
    ::testing::Values(
        genoTestValues(genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::JBIG, "/data/records/conformance/1.3.5.bgz.CASE01"),
        genoTestValues{genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::JBIG,
                       "/data/records/conformance/ALL.chrY.phase3_integrated_v2a.20130502.genotypes.bgz.CASE02"},
        genoTestValues{genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::JBIG, "/data/records/conformance/1.3.11.bgz.CASE03"},
        genoTestValues{genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::JBIG, "/data/records/conformance/1.3.11.bgz.CASE04"},
        genoTestValues{genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::ROW_BIN,
                       genie::core::AlgoID::JBIG, "/data/records/conformance/1.3.11.bgz.CASE03"},
        genoTestValues{genie::genotype::SortingAlgoID::RANDOM_SORT, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::JBIG, "/data/records/conformance/1.3.11.bgz.CASE03"},
        genoTestValues{genie::genotype::SortingAlgoID::NO_SORTING, genie::genotype::BinarizationID::BIT_PLANE,
                       genie::core::AlgoID::BSC, "/data/records/conformance/1.3.11.bgz.CASE03"}));

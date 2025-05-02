/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "genie/annotation/annotation.h"
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"
#include "helpers.h"

struct ContactTestDetails {
  ContactTestDetails(std::string casename, std::string contactin,
                     bool REMOVE_UNALIGNED_REGION, bool TRANSFORM_MASK,
                     bool ENA_DIAG_TRANSFORM, bool ENA_BINARIZATION,
                     bool NORM_AS_WEIGHT, bool MULTIPLICATIVE_NORM,
                     uint32_t TILE_SIZE, uint32_t MULT = 1u){//, std::vector<uint32_t> BINSIZEMULT ={}) {
    filename = contactin;
    cm_pars.REMOVE_UNALIGNED_REGION = REMOVE_UNALIGNED_REGION;
    cm_pars.TRANSFORM_MASK = TRANSFORM_MASK;
    cm_pars.ENA_DIAG_TRANSFORM = ENA_DIAG_TRANSFORM;
    cm_pars.ENA_BINARIZATION = ENA_BINARIZATION;
    cm_pars.NORM_AS_WEIGHT = NORM_AS_WEIGHT;
    cm_pars.MULTIPLICATIVE_NORM = MULTIPLICATIVE_NORM;
    cm_pars.CODEC_ID = genie::core::AlgoID::JBIG, cm_pars.TILE_SIZE = TILE_SIZE;
    cm_pars.MULT = MULT;
    outputfile = casename;
    //binSizemultipliers = BINSIZEMULT;
  }

  std::string filename;
  std::string outputfile;
  genie::annotation::ContactMatrixParameters cm_pars;
 // std::vector<uint32_t> binSizemultipliers;
};

class AnnotationContactTests
    : public ::testing::TestWithParam<ContactTestDetails> {
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

TEST_P(AnnotationContactTests, contactparameterset) {
  std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
  std::string filePath = gitRootDir + "/data/records/contact/";
  auto testParams = GetParam();
  std::string inputFilename = filePath + testParams.filename;

  ASSERT_TRUE(std::filesystem::exists(inputFilename));

  std::string outputFilename = filePath + testParams.outputfile + "-output";
  std::filesystem::remove(outputFilename + ".bin");

  genie::annotation::Compressor compressors;
  std::string comment = "# with parameters";
  std::string set1 = "compressor 1 1 BSC {32 128 1 1}";
  std::string set2 = "compressor 3 2 LZMA {8 16777216 3 0 2 32}";
  std::string set3 = "compressor 1 2 ZSTD {0 0}";
  std::stringstream config;
  config << set1 << '\n' << set2 << '\n' << set3 << '\n';

  genie::annotation::ContactMatrixParameters cmParameters = testParams.cm_pars;

  genie::annotation::Annotation annotationGenerator;
  annotationGenerator.setTileSize(100, 3000);
  annotationGenerator.setContactOptions(cmParameters);
  annotationGenerator.setCompressorConfig(config);
  annotationGenerator.startStream(genie::annotation::RecType::CM_FILE,
                                  inputFilename, outputFilename);
  EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin"))
      << outputFilename + ".bin";
}

// contactin
// REMOVE_UNALIGNED_REGION, TRANSFORM_MASK,  ENA_DIAG_TRANSFORM,
// ENA_BINARIZATION, NORM_AS_WEIGHT,  MULTIPLICATIVE_NORM,TILE_SIZE
INSTANTIATE_TEST_SUITE_P(
    testoutputs, AnnotationContactTests,
    ::testing::Values(
        ContactTestDetails(
            "CASE01",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            false, false, true, true, true, false, 1000),
        ContactTestDetails(
            "CASE02",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            false, false, true, true, true, false, 150),
        ContactTestDetails(
            "CASE03",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont",
            false, false, true, true, true, false, 1000),
        ContactTestDetails(
            "CASE04",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont",
            false, false, true, true, true, false, 150),
        ContactTestDetails(
            "CASE07",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_21.cont",
            false, false, true, true, true, false, 1000, 5u),
        ContactTestDetails(
            "CASE08",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_21.cont",
            false, false, true, true, true, false, 150, 5u),//, {1,5}),
        ContactTestDetails(
            "CASE09",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            true, false, true, true, true, false, 1000),
        ContactTestDetails(
            "CASE12",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            true, false, false, true, true, false, 1000),
        ContactTestDetails(
            "CASE13",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            true, false, true, true, true, false, 1000),
        ContactTestDetails(
            "CASE14",
            "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont",
            true, false, true, true, true, false, 1000)));

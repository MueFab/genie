/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/annotation.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "helpers.h"

struct TestDetails {
  TestDetails(std::string genoin, std::string sitein, uint64_t totRows,
              uint32_t tileHeigth, uint32_t tileWidth)
      : genofile_in(genoin),
        sitefile_in(sitein),
        totNrOfRows(totRows),
        defaultTileHeight(tileHeigth),
        defaultTileWidth(tileWidth) {}
  std::string genofile_in;
  std::string sitefile_in;
  uint64_t totNrOfRows;
  uint32_t defaultTileHeight;
  uint32_t defaultTileWidth;
};

class AnnotationTests : public ::testing::TestWithParam<TestDetails> {
 protected:
  // Do any necessary setup for your tests here
  AnnotationTests() = default;

  ~AnnotationTests() override = default;

  // Use SetUp instead of the constructor in the following cases:
  // - In the body of a constructor (or destructor), it's not possible to
  //   use the ASSERT_xx macros. Therefore, if the set-up operation could
  //   cause a fatal test failure that should prevent the test from running,
  //   it's necessary to use a CHECK macro or to use SetUp() instead of a
  //   constructor.
  // - If the tear-down operation could throw an exception, you must use
  //   TearDown() as opposed to the destructor, as throwing in a destructor
  //   leads to undefined behavior and usually will kill your program right
  //   away. Note that many standard libraries (like STL) may throw when
  //   exceptions are enabled in the compiler. Therefore you should prefer
  //   TearDown() if you want to write portable tests that work with or
  //   without exceptions.
  // - The googletest team is considering making the assertion macros throw
  //   on platforms where exceptions are enabled (e.g. Windows, Mac OS, and
  //   Linux client-side), which will eliminate the need for the user to
  //   propagate failures from a subroutine to its caller. Therefore, you
  //   shouldn't use googletest assertions in a destructor if your code
  //   could run on such a platform.
  // - In a constructor or destructor, you cannot make a virtual function
  //   call on this object. (You can call a method declared as virtual, but
  //   it will be statically bound.) Therefore, if you need to call a method
  //   that will be overridden in a derived class, you have to use
  //   SetUp()/TearDown().

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

// compressor compressorID stepID algorithmID {parameter1 parameter2 ...
// parametern} {invar1 invar2 ... invarn} {outvar1 outvar2 ... outvarn}
TEST_F(AnnotationTests, compressorConfig) {
  genie::annotation::Compressor compressors;
  std::string comment = "# 23fnv0-33 bla";
  std::string set1 = "compressor 1 1 BSC";
  std::string set2 = "compressor 2 2 LZMA";
  std::stringstream config;
  config << set1 << '\n' << set2 << '\n';
  compressors.parseConfig(config);
  EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}

TEST_F(AnnotationTests, compressorConfigSteps) {
  genie::annotation::Compressor compressors;
  std::string comment = "# with parameters";
  std::string set1 = "compressor 1 1 BSC {32 128 1 1}";
  std::string set2 = "compressor 2 2 LZMA {8 16777216 3 0 2 32}";
  std::string set3 = "compressor 2 1 ZSTD {0 0}";
  std::stringstream config;
  config << set1 << '\n' << set2 << '\n' << set3 << '\n';
  compressors.parseConfig(config);
  EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}
TEST_F(AnnotationTests, compressorConfigcompressors) {
  genie::annotation::Compressor compressors;
  std::string comment = "# with parameters";
  std::string set1 = "compressor 1 1 BSC";
  std::string set2 = "compressor 3 2 LZMA {8 16777216 3 0 2 32}";
  std::string set3 = "compressor 3 1 ZSTD {0 0}";
  std::stringstream config;
  config << set1 << '\n' << set2 << '\n' << set3 << '\n';
  compressors.parseConfig(config);
  EXPECT_EQ(compressors.getNrOfCompressorIDs(), 2);
}

const bool RUNBIGFILES = true;

static std::string composeGenoOutputFileName(std::string path,
                                             TestDetails params) {
  std::string orgfilename = std::filesystem::path(params.genofile_in).string();
  size_t lastindex = orgfilename.find_last_of(".");
  orgfilename = orgfilename.substr(0, lastindex);
  lastindex = orgfilename.find_last_of(".")+1;
  orgfilename = path + orgfilename.substr(0, lastindex);
  std::string outputFilename = orgfilename + "R" + std::to_string(params.totNrOfRows);
  outputFilename += "-C1092_TS" + std::to_string(params.defaultTileHeight) +
                    "-" + std::to_string(params.defaultTileWidth) + "_geno";
  return outputFilename;
}

static std::string composeSiteOutputFileName(std::string path,
                                             TestDetails params) {
  std::string orgfilename = std::filesystem::path(params.sitefile_in).string();
  size_t lastindex = orgfilename.find_last_of(".");
  orgfilename = orgfilename.substr(0, lastindex);
  lastindex = orgfilename.find_last_of(".")+1;
  orgfilename = path+ orgfilename.substr(0, lastindex);

  std::string outputFilename =
      orgfilename + "R" + std::to_string(params.totNrOfRows);
  outputFilename +=
      "-Cn_TS" + std::to_string(params.defaultTileHeight) + "_site";
  return outputFilename;
}

TEST_P(AnnotationTests, annotationSite) {
  auto testParams = GetParam();

  std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
  std::string filePath = gitRootDir + "/data/records/annotation/";
  std::string inputFilename =
      filePath + testParams.sitefile_in;  // "ALL.chrX.10000.site";

  ASSERT_TRUE(std::filesystem::exists(inputFilename));
  if constexpr (!RUNBIGFILES)
    if (std::filesystem::file_size(inputFilename) > 100 * 1024) return;

  std::string outputFilename = composeSiteOutputFileName(filePath, testParams);
                                 
  std::filesystem::remove(outputFilename + ".bin");

  genie::annotation::Annotation annotationGenerator;
  std::string comment = "# with parameters";
  std::string set1 = "compressor 1 1 BSC";
  std::string set2 = "compressor 2 1 LZMA {8 16777216 3 0 2 32}";
  std::string set3 = "compressor 3 1 ZSTD {0 0}";
  std::stringstream config;
  config << set1 << '\n';  //    << set2 << '\n' << set3 << '\n';

  annotationGenerator.setCompressorConfig(config);
  annotationGenerator.setTileSize(testParams.defaultTileHeight,
                                  testParams.defaultTileWidth);
  annotationGenerator.startStream(genie::annotation::RecType::SITE_FILE,
                                  inputFilename, outputFilename);

  EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin"));
}

TEST_P(AnnotationTests, annotationGeno) {
  std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
  std::string filePath = gitRootDir + "/data/records/annotation/";
  auto testParams = GetParam();

  std::string inputFilename =
      filePath + testParams.genofile_in;  //"ALL.chrX.10000.geno";


  ASSERT_TRUE(std::filesystem::exists(inputFilename));

  auto inputfilesize = std::filesystem::file_size(inputFilename);
  if constexpr (!RUNBIGFILES)
    if (inputfilesize > 50 * 1024 * 1024) return;

  std::string outputFilename = composeGenoOutputFileName(filePath, testParams);

  std::filesystem::remove(outputFilename + ".bin");

  std::string comment = "# with parameters";
  std::string set1 = "compressor 1 1 BSC";
  std::string set2 = "compressor 3 2 LZMA {8 16777216 3 0 2 32}";
  std::string set3 = "compressor 3 1 ZSTD {0 0}";
  std::stringstream config;
  config << set1 << '\n';  //   << set2 << '\n' << set3 << '\n';

  uint32_t BLOCK_SIZE = testParams.defaultTileHeight;
  bool TRANSFORM_MODE = true;

  genie::likelihood::EncodingOptions likelihood_opt = {
      BLOCK_SIZE,      // block_size
      TRANSFORM_MODE,  // transform_flag;
  };

  genie::genotype::EncodingOptions genotype_opt = {
      BLOCK_SIZE,                                  // block_size;
      genie::genotype::BinarizationID::ROW_BIN,    // BIT_PLANE,  //
                                                   // binarization_ID;
      genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
      false,                                       // transpose_mat;
      genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
      genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
      genie::core::AlgoID::JBIG                    // codec_ID_;
  };

  ASSERT_FALSE(!std::filesystem::is_regular_file(inputFilename));

  genie::annotation::Annotation annotationGenerator;
  annotationGenerator.setLikelihoodOptions(likelihood_opt);
  annotationGenerator.setGenotypeOptions(genotype_opt);
  annotationGenerator.setCompressorConfig(config);
  annotationGenerator.setTileSize(testParams.defaultTileHeight,
                                  testParams.defaultTileWidth);
  std::cerr << "starting stream..." << std::endl;
  annotationGenerator.startStream(genie::annotation::RecType::GENO_FILE,
                                  inputFilename, outputFilename);
  std::cerr << "returning stream..." << std::endl;

  EXPECT_TRUE(std::filesystem::exists(outputFilename + ".bin"));
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, AnnotationTests,
    ::testing::Values(
        TestDetails("ALL.chrX.10000.geno", "ALL.chrX.10000.site", 10000u, 1000, 3000u),
                      TestDetails("ALL.chrX.15.geno", "ALL.chrX.15.site", 15u, 15u, 3000u),
                      TestDetails("ALL.chrX.15.geno", "ALL.chrX.15.site", 15u, 4u, 3000u)));

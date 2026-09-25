/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// End-to-end annotation tests for all untested RecType handlers:
//   SAMPLE_FILE, FEATURE_FILE, GENE_EXPRESSION_FILE,
//   FUNCTIONAL_ANNOTATIONS_FILE, TRACK_FILE, TRACK_PROPERTY_FILE

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>

#include "genie/variant_site/annotation.h"
#include "helpers.h"

// Set to true to also run tests on large (>100 KB) input files.
constexpr bool RUNBIGFILES = false;

// ---------------------------------------------------------------------------
// Helper: git root path
// ---------------------------------------------------------------------------
static std::string gitRoot() {
    return util_tests::exec("git rev-parse --show-toplevel");
}

// ---------------------------------------------------------------------------
// SampleAnnotation — SAMPLE_FILE
// ---------------------------------------------------------------------------

struct SampleTestDetails {
    std::string filename;   ///< Input file relative to data/records/expression/
    std::string outputTag;  ///< Unique output file stem
    uint32_t tileHeight{100};
    uint32_t tileWidth{3000};
};

class SampleAnnotationTests : public ::testing::TestWithParam<SampleTestDetails> {};

TEST_P(SampleAnnotationTests, sampleParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/expression/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, p.tileWidth);
    gen.startStream(genie::annotation::RecType::SAMPLE_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, SampleAnnotationTests,
    ::testing::Values(
        SampleTestDetails{"5.4.1-tp01.sample", "sample_CASE01", 100u, 3000u}));

// ---------------------------------------------------------------------------
// FeatureAnnotation — FEATURE_FILE
// ---------------------------------------------------------------------------

struct FeatureTestDetails {
    std::string filename;
    std::string outputTag;
    uint32_t tileHeight{100};
};

class FeatureAnnotationTests : public ::testing::TestWithParam<FeatureTestDetails> {};

TEST_P(FeatureAnnotationTests, featureParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/expression/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;
    if constexpr (!RUNBIGFILES)
        if (std::filesystem::file_size(inputFile) > 100 * 1024)
            GTEST_SKIP() << "Skipping large file (set RUNBIGFILES=true to enable)";

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, 0u);
    gen.startStream(genie::annotation::RecType::FEATURE_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, FeatureAnnotationTests,
    ::testing::Values(
        FeatureTestDetails{"5.4.1-tp01.feat", "feature_CASE01", 100u}));

// ---------------------------------------------------------------------------
// GeneExpressionAnnotation — GENE_EXPRESSION_FILE
// ---------------------------------------------------------------------------

struct GeneExpressionTestDetails {
    std::string filename;   ///< .atr combined attribute record file
    std::string outputTag;
    uint32_t tileHeight{100};
    uint32_t tileWidth{3000};
};

class GeneExpressionAnnotationTests : public ::testing::TestWithParam<GeneExpressionTestDetails> {};

TEST_P(GeneExpressionAnnotationTests, geneExpressionParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/expression/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;
    if constexpr (!RUNBIGFILES)
        if (std::filesystem::file_size(inputFile) > 100 * 1024)
            GTEST_SKIP() << "Skipping large file (set RUNBIGFILES=true to enable)";

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, p.tileWidth);
    gen.startStream(genie::annotation::RecType::GENE_EXPRESSION_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, GeneExpressionAnnotationTests,
    ::testing::Values(
        GeneExpressionTestDetails{"5.4.1-tp01.atr", "geneexpr_CASE01", 100u, 3000u}));

// ---------------------------------------------------------------------------
// FunctionalAnnotation — FUNCTIONAL_ANNOTATIONS_FILE
// ---------------------------------------------------------------------------

struct FunctionalTestDetails {
    std::string filename;
    std::string outputTag;
    uint32_t tileHeight{1000};
};

class FunctionalAnnotationTests : public ::testing::TestWithParam<FunctionalTestDetails> {};

TEST_P(FunctionalAnnotationTests, functionalParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/functional_annotation/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;
    if constexpr (!RUNBIGFILES)
        if (std::filesystem::file_size(inputFile) > 100 * 1024)
            GTEST_SKIP() << "Skipping large file (set RUNBIGFILES=true to enable)";

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, 0u);
    gen.startStream(genie::annotation::RecType::FUNCTIONAL_ANNOTATIONS_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, FunctionalAnnotationTests,
    ::testing::Values(
        FunctionalTestDetails{"FA01_Homo_sapiens.GRCh38.95.chr.gtf.mgrec", "functional_CASE01", 1000u},
        FunctionalTestDetails{"FA02_Homo_sapiens.GRCh38.95.chr.gtf.mgrec", "functional_CASE02", 1000u},
        FunctionalTestDetails{"FA03_Homo_sapiens.GRCh38.95.gff3.mgrec",    "functional_CASE03", 1000u},
        FunctionalTestDetails{"FA04_Homo_sapiens.GRCh38.95.gff3.mgrec",    "functional_CASE04", 1000u}));

// ---------------------------------------------------------------------------
// TrackDataAnnotation — TRACK_FILE
// ---------------------------------------------------------------------------

struct TrackDataTestDetails {
    std::string filename;
    std::string outputTag;
    uint32_t tileHeight{1000};
};

class TrackDataAnnotationTests : public ::testing::TestWithParam<TrackDataTestDetails> {};

TEST_P(TrackDataAnnotationTests, trackDataParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/track/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;
    if (std::filesystem::file_size(inputFile) > 50 * 1024 * 1024) {
        GTEST_SKIP() << "Skipping large file in non-RUNBIGFILES mode";
    }

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, 0u);
    gen.startStream(genie::annotation::RecType::TRACK_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, TrackDataAnnotationTests,
    ::testing::Values(
        TrackDataTestDetails{"TD01_snATAC_-_Endo_Lympho_data.mgrec",        "trackdata_CASE01", 1000u},
        TrackDataTestDetails{"TD02_concat_KGS_Hg19_Chr1_50_data.mgrec",     "trackdata_CASE02", 1000u},
        TrackDataTestDetails{"Kidney_Genetic_Scorecard_INT_Hg19_Chr1_data.mgrec", "trackdata_CASE03", 1000u},
        TrackDataTestDetails{"Kidney_Genetic_Scorecard_WIG_Hg19_Chr1_data.mgrec", "trackdata_CASE04", 1000u},
        TrackDataTestDetails{"trfMask_bed.1000.mgrec",                       "trackdata_CASE05", 1000u}));

// ---------------------------------------------------------------------------
// TrackPropertyAnnotation — TRACK_PROPERTY_FILE
// ---------------------------------------------------------------------------

struct TrackPropertyTestDetails {
    std::string filename;
    std::string outputTag;
    uint32_t tileHeight{1000};
};

class TrackPropertyAnnotationTests : public ::testing::TestWithParam<TrackPropertyTestDetails> {};

TEST_P(TrackPropertyAnnotationTests, trackPropertyParameterSet) {
    auto p = GetParam();
    std::string path = gitRoot() + "/data/records/track/";
    std::string inputFile = path + p.filename;

    ASSERT_TRUE(std::filesystem::exists(inputFile)) << "Missing: " << inputFile;

    std::string outputFile = path + p.outputTag + "-output";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");

    genie::annotation::Annotation gen;
    std::stringstream cfg;
    cfg << "compressor 1 1 BSC {32 128 1 1}\n";
    gen.setCompressorConfig(cfg);
    gen.setTileSize(p.tileHeight, 0u);
    gen.startStream(genie::annotation::RecType::TRACK_PROPERTY_FILE, inputFile, outputFile);

    EXPECT_TRUE(std::filesystem::exists(outputFile + ".bin")) << outputFile + ".bin";
    std::filesystem::remove(outputFile + ".bin");
    std::filesystem::remove(outputFile + ".txt");
}

INSTANTIATE_TEST_SUITE_P(
    testoutputs, TrackPropertyAnnotationTests,
    ::testing::Values(
        TrackPropertyTestDetails{"TD01_snATAC_-_Endo_Lympho_prop.mgrec",    "trackprop_CASE01", 1000u},
        TrackPropertyTestDetails{"TD02_concat_KGS_Hg19_Chr1_50_prop.mgrec", "trackprop_CASE02", 1000u},
        TrackPropertyTestDetails{"Kidney_Genetic_Scorecard_INT_Hg19_Chr1_prop.mgrec", "trackprop_CASE03", 1000u},
        TrackPropertyTestDetails{"Kidney_Genetic_Scorecard_WIG_Hg19_Chr1_prop.mgrec", "trackprop_CASE04", 1000u}));

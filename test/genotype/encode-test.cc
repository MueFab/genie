/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <codecs/include/mpegg-codecs.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <vector>
#include <xtensor/xmath.hpp>
#include <xtensor/xoperation.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xview.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/variantsite/AccessUnitComposer.h"

class GenotypeTest : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    GenotypeTest() = default;

    ~GenotypeTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};
TEST(Genotype, Decompose) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO(Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);

    ASSERT_EQ(block.max_ploidy, 2);
    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);

    auto& allele_mat = block.allele_mat;
    auto& phasing_mat = block.phasing_mat;

    // Check allele_mat shape
    ASSERT_EQ(allele_mat.dimension(), 2);
    ASSERT_EQ(allele_mat.shape(0), 100);
    ASSERT_EQ(allele_mat.shape(1), 1092 * 2);
    ASSERT_EQ(allele_mat(0, 3), 1);

    // Check phasing_mat shape
    ASSERT_EQ(phasing_mat.dimension(), 2);
    ASSERT_EQ(phasing_mat.shape(0), 100);
    ASSERT_EQ(phasing_mat.shape(1), 1092);

    // Check all values
    ASSERT_EQ(xt::amin(allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(allele_mat)(0), 1);

    // Check the content of the first row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, 0, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, 0LL, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2067);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 117);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    // Check the content of the last row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, -1, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, -1LL, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2178);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 6);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    //------------------------------------------------------
}

TEST(Genotype, formattest) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/ALL.chrX.5000.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();
    std::vector<std::string> formatList;
    {
        // TODO(Stefanie): File does not exist in the repository
        std::string jsonfilepath =
            gitRootDir +
            "/data/records/ALL.chrX.phase1_release_v3.20101123.snps_indels_svs.genotypes.vcf.formattags.json";
        std::ifstream jsonReader(jsonfilepath, std::ios::in);
        ASSERT_TRUE(jsonReader.is_open());
        std::string stringFields((std::istreambuf_iterator<char>(jsonReader)), std::istreambuf_iterator<char>());
        stringFields.erase(std::remove(stringFields.begin(), stringFields.end(), ' '), stringFields.end());
        stringFields.erase(std::remove(stringFields.begin(), stringFields.end(), '\t'), stringFields.end());
        stringFields.erase(std::remove(stringFields.begin(), stringFields.end(), '"'), stringFields.end());
        stringFields = stringFields.substr(1, stringFields.size() - 2);
        size_t pos = 0;
        std::string token;
        while ((pos = stringFields.find(",")) != std::string::npos) {
            formatList.emplace_back(stringFields.substr(0, pos));
            stringFields.erase(0, pos + 1);
        }
        formatList.emplace_back(stringFields);
    }

    EXPECT_EQ(formatList.size(), 3);
}

#define WRITETESTFILES
std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    genie::genotype::EncodingOptions opt = {
        blockSize,                                   // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    return genie::genotype::encode_block(opt, recs);
}
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    bool TRANSFORM_MODE = true;
    genie::likelihood::EncodingOptions opt = {
        blockSize,       // block_size
        TRANSFORM_MODE,  // transform_flag;
    };
    genie::likelihood::EncodingBlock block;
    genie::likelihood::extract_likelihoods(opt, block, recs);
    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;
    genie::likelihood::serialize_mat(block.idx_mat, block.dtype_id, block.nrows, block.ncols, block.serialized_mat);
    genie::likelihood::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    block.serialized_mat.seekp(0, std::ios::end);

    genie::entropy::lzma::LZMAEncoder lzmaEncoder;
    std::stringstream compressedData;
    lzmaEncoder.encode(block.serialized_arr, compressedData);

    block.serialized_arr.str("");
    block.serialized_arr << compressedData.rdbuf();
    compressedData.str("");

    lzmaEncoder.encode(block.serialized_mat, compressedData);
    block.serialized_mat.str("");
    block.serialized_mat << compressedData.rdbuf();

    genie::likelihood::LikelihoodParameters parameters(static_cast<uint8_t>(recs.at(0).getNumberOfLikelihoods()),
                                                       TRANSFORM_MODE, block.dtype_id);
    return std::make_tuple(parameters, block);
}


TEST_P(GenotypeTest, conformanceTests) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename;
    // std::string filepath = gitRootDir + "/data/records/conformance/1.3.5.bgz.CASE01.geno";
    // std::string filepath = gitRootDir + "/data/records/conformance/1.3.11.bgz.CASE03.geno";
    // std::string filepath = gitRootDir + "/data/records/conformance/1.3.11.bgz.CASE04.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();
    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 3);

    uint32_t BLOCK_SIZE = 200;

    auto genotypeData = genotypeEncoding(BLOCK_SIZE, recs);
    auto likelihoodData = likelihoodEncoding(BLOCK_SIZE, recs);
    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters =
        std::get<genie::genotype::GenotypeParameters>(genotypeData);
    auto datablock = std::get<genie::genotype::EncodingBlock>(genotypeData);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters,
                                   std::get<genie::likelihood::LikelihoodParameters>(likelihoodData), recs.size());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    for (auto formatdata : datablock.attributeData) {
        auto& info = attributesInfo[formatdata.first];
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(std::min(BLOCK_SIZE, static_cast<uint32_t>(recs.size())));
        arrayDims.push_back(recs.at(0).getNumSamples());
        arrayDims.push_back(recs.at(0).getFormatCount());
        attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                arrayDims);
        attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }

    descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
    {
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }

    // add LINK_ID default values
    for (auto i = 0; i < BLOCK_SIZE && i < recs.size(); ++i) {
        char val = 0xFF;
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

#ifdef WRITETESTFILES
    if (false) {
        std::string name = filepath + "_AAU";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            AAU_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            AAU_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
    if (false) {
        std::string name = filepath + "_APS";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            APS_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            APS_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
    {
        std::string name = filepath + "_output";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            APS_dataUnit.write(writer);
            AAU_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            APS_dataUnit.write(txtWriter);
            AAU_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
#endif
}
INSTANTIATE_TEST_CASE_P(testallConformance, GenotypeTest,
                         ::testing::Values("/data/records/conformance/1.3.5.bgz.CASE01.geno",
                                           "/data/records/conformance/1.3.11.bgz.CASE03.geno",
                                           "/data/records/conformance/1.3.11.bgz.CASE04.geno"));

TEST(Genotype, AdaptiveMaxValue) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);

    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);
    ASSERT_EQ(xt::amin(block.allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(block.allele_mat)(0), 1);
}

TEST(Genotype, BinarizeBitPlane) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    // Check DO_NOT_CONCAT
    {
        genie::genotype::EncodingOptions opt = {
            512,                                         // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
            false,                                       // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
            genie::core::AlgoID::JBIG                    // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_COL_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_COL_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }
}

TEST(Genotype, BinarizeRowBin) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::ROW_BIN,    // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);
    genie::genotype::binarize_allele_mat(opt, block);

    // TODO @Yeremia: Update the assert
    ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
    ASSERT_EQ(block.allele_bin_mat_vect[0].shape(0), static_cast<size_t>(xt::sum(block.amax_vec)(0)));
    //    ASSERT_EQ(block.allele_bin_mat_vect.shape(2), block.allele_mat.shape(1));
}

TEST(Genotype, RandomSort) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }
}

TEST(Genotype, Serializer) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t bitmap[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2, 0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    size_t ncols = 23;
    size_t nrows = 5;

    genie::genotype::bin_mat_from_bytes(bin_mat, bitmap, orig_payload_len, nrows, ncols);

    std::vector<uint8_t> temp;
    for (auto byte : bin_mat) temp.push_back(byte);

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(bin_mat, &payload, payload_len);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(bitmap + i));
    }
}

#include "genie/entropy/jbig/encoder.h"

TEST(Genotype, JBIG) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t orig_payload[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    unsigned long orig_ncols = 23;
    unsigned long orig_nrows = 5;

    uint8_t orig_compressed[] = {0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
                                 0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2};

    uint8_t* compressed_data;

    size_t compressed_data_len;

    mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, orig_payload, orig_payload_len, orig_nrows,
                                orig_ncols);

    for (size_t i = 0; i < compressed_data_len; ++i) {
        EXPECT_EQ(orig_compressed[i], compressed_data[i]);
    }

    std::stringstream uncomressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : orig_payload) uncomressed_input.write((char*)&byte, 1);
    genie::entropy::jbig::JBIGEncoder encoder;
    encoder.encode(uncomressed_input, compressed_output, (uint32_t)orig_ncols, (uint32_t)orig_nrows);

    std::vector<uint8_t> mem_data;
    for (auto byte : compressed_output.str()) {
        mem_data.push_back(byte);
    }
    for (size_t i = 0; i < compressed_data_len; ++i) {
        EXPECT_EQ(orig_compressed[i], mem_data[i]);
    }

    std::stringstream uncompressed_output;
    uint32_t output_ncols = 0;
    uint32_t output_nrows = 0;
    encoder.decode(compressed_output, uncompressed_output, output_ncols, output_nrows);

    std::vector<uint8_t> mem_data_source(3 * orig_payload_len);
    uint8_t* payload = &mem_data_source[0];
    size_t payload_len;

    unsigned long ncols;
    unsigned long nrows;
    mpegg_jbig_decompress_default(&payload, &payload_len, compressed_data, compressed_data_len, &nrows, &ncols);

    ASSERT_EQ(nrows, orig_nrows);
    ASSERT_EQ(ncols, orig_ncols);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(orig_payload + i));
    }
}

TEST(Genotype, genoAndLikelihood) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/ALL.chrX.5000.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();
    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 5000);

    uint32_t BLOCK_SIZE = 200;

    auto genotypeData = genotypeEncoding(BLOCK_SIZE, recs);
    auto likelihoodData = likelihoodEncoding(BLOCK_SIZE, recs);
    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters =
        std::get<genie::genotype::GenotypeParameters>(genotypeData);
    auto datablock = std::get<genie::genotype::EncodingBlock>(genotypeData);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters,
                                   std::get<genie::likelihood::LikelihoodParameters>(likelihoodData), recs.size());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    for (auto formatdata : datablock.attributeData) {
        auto& info = attributesInfo[formatdata.first];
        auto arraylength = info.getArrayLength();
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(BLOCK_SIZE);
        for (auto i = 1; i < arraylength; ++i) arrayDims.push_back(2);
        attributeTDStream[formatdata.first].set(info.getAttributeType(), info.getArrayLength(), arrayDims);
    }
    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }
    descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
    {
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::ofstream testfile;
    testfile.open(filepath + ".bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filepath + ".txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);

    APS_dataUnit.write(testwriter);
    APS_dataUnit.write(txtwriter);

    AAU_dataUnit.write(testwriter);
    AAU_dataUnit.write(txtwriter);
    testfile.close();
    txtfile.close();
}
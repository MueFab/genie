/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/arrayType.h"
#include "genie/util/string-helpers.h"

#include "genie/annotation/AnnotationEncoder.h"
#include "genie/annotation/ParameterSetComposer.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"
#include "genie/variantsite/VariantSiteParser.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/util/bitreader.h"

class VariantSiteRecordTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    VariantSiteRecordTests() = default;

    ~VariantSiteRecordTests() override = default;

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

TEST_F(VariantSiteRecordTests, readFilefrombin) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.10000";
    std::ifstream inputfile;
    inputfile.open(path + filename + ".site", std::ios::in | std::ios::binary);

    if (inputfile.is_open()) {
        genie::util::BitReader reader(inputfile);
        std::ofstream outputfile(path + filename + "_site.txt");
        genie::core::Writer txtwriter(&outputfile, true);
        do {
            genie::core::record::variant_site::Record variant_site_record(reader);
            variant_site_record.write(txtwriter);
        } while (inputfile.peek() != EOF);
        inputfile.close();
        outputfile.close();
    }
}
namespace util_tests {

std::string exec(const std::string& cmd) {
#ifdef WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) {
        return "<exec(" + cmd + ") failed>";
    }

    const int bufferSize = 256;
    char buffer[bufferSize];
    std::string result;

    while (!feof(pipe)) {
        if (fgets(buffer, bufferSize, pipe) != nullptr) {
            result += buffer;
        }
    }
#ifdef WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    genie::util::rtrim(result);

    return result;
}

}  // namespace util_tests

TEST_F(VariantSiteRecordTests, DISABLED_readFileRunParser) {  // NOLINT(cert-err58-cpp)

    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/";
    std::string filename = "ALL.chrX.10000.site";

    constexpr size_t ExpectedNumberOfrows = 1000;
    std::ifstream inputfile;
    inputfile.open(filepath + filename, std::ios::in | std::ios::binary);

    std::string infofilename = "1.3.5.header100.gt_only.vcf.infotags.json";
    std::ifstream infoFieldsFile;
    infoFieldsFile.open(filepath + infofilename, std::ios::in);
    std::stringstream infoFields;
    if (infoFieldsFile.is_open()) {
        infoFields << infoFieldsFile.rdbuf();
        infoFieldsFile.close();
    }

    std::string set1 = "compressor 1 0 BSC";
    std::string set2 = "compressor 1 1 LZMA";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Compressor compressors;
    compressors.parseConfig(config);

    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, 0);
    ASSERT_TRUE(inputfile.is_open());
    if (inputfile.is_open()) {
        EXPECT_GE(parser.getNumberOfRows(), ExpectedNumberOfrows);
        inputfile.close();
    }

    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    ASSERT_EQ(parser.getNrOfTiles(), 1);
    auto info = parser.getAttributes().getInfo();
    std::map<genie::core::AnnotDesc, std::stringstream> desc;
    std::map<std::string, std::stringstream> attr;

    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        for (auto& desctile : tile_descriptorStream) {
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();
        }

        for (auto& attrtile : tile_attributeStream) {
            attr[attrtile.first] << attrtile.second.getTile(i).rdbuf();
        }
    }
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

    genie::annotation::AnnotationEncoder annotationEncodecomposer;
    annotationEncodecomposer.setAttributes(info);
    annotationEncodecomposer.setCompressors(compressors);
    annotationEncodecomposer.setDescriptors(descrList);
    auto annotationEncondingParameters = annotationEncodecomposer.Compose();
    std::pair<uint64_t, uint64_t> tile_Size = {parser.getNumberOfRows(), 0u};
    genie::annotation::ParameterSetComposer parametersSetcomposer;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet= parametersSetcomposer.Compose(
        AT_ID, AG_class, tile_Size, annotationEncondingParameters);

 //   genie::variant_site::ParameterSetComposer encodeParameters;
  //  genie::core::record::annotation_parameter_set::Record annotationParameterSet = encodeParameters.setParameterSet(
  //      descrList, info, compressors.getCompressorParameters(), parser.getNumberOfRows(), 1);

    //----------------------------------------------------//
    genie::variant_site::AccessUnitComposer accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    uint64_t rowIndex = 0;

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTileStream;
    accessUnit.setCompressors(compressors);
    accessUnit.setAccessUnit(desc, attributeTileStream, info, annotationParameterSet, annotationAccessUnit, AG_class,
                             AT_ID, rowIndex);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::string name = filepath + filename;

    std::ofstream testfile;
    testfile.open(name + "_1tile_typed_data.bin", std::ios::binary | std::ios::out);
    if (testfile.is_open()) {
        genie::core::Writer writer(&testfile);
        APS_dataUnit.write(writer);
        AAU_dataUnit.write(writer);
        testfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + "_1tile_typed_data.txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtWriter(&txtfile, true);
        APS_dataUnit.write(txtWriter);
        AAU_dataUnit.write(txtWriter);
        txtfile.close();
    }
}

TEST_F(VariantSiteRecordTests, DISABLED_multitile) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/ALL.chrX.10000.site";
    std::string infofilename = "/data/records/1.3.5.header100.gt_only.vcf.infotags.json";
    std::ifstream infoFieldsFile;
    infoFieldsFile.open(gitRootDir + infofilename, std::ios::in);
    std::stringstream infoFields;
    if (infoFieldsFile.is_open()) {
        infoFields << infoFieldsFile.rdbuf();
        infoFieldsFile.close();
    }
    uint8_t AT_ID = 1;
    std::ifstream inputfile;
    inputfile.open(filepath, std::ios::in | std::ios::binary);
    uint64_t defaultTileSize = 1000;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);
    if (inputfile.is_open()) inputfile.close();
    EXPECT_EQ(parser.getNrOfTiles(), 10);

    std::string set1 = "compressor 1 0 BSC";
    std::string set2 = "compressor 1 1 LZMA";
    std::string set3 = "compressor 2 0 ZSTD";
    std::string set4 = "compressor 3 0 BSC";
    std::stringstream config;
    config << set1 << '\n' << set3 << '\n' << set4 << '\n';

    genie::annotation::Compressor compressors;
    compressors.parseConfig(config);

    auto info = parser.getAttributes().getInfo();
    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    genie::variant_site::ParameterSetComposer encodeParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet = encodeParameters.setParameterSet(
        descrList, info, compressors.getCompressorParameters(), defaultTileSize, AT_ID);

    uint8_t AG_class = 1;

    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit(parser.getNrOfTiles());
    uint64_t rowIndex = 0;
    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<genie::core::AnnotDesc, std::stringstream> desc;
        for (auto& desctile : tile_descriptorStream) {
            ASSERT_GE(desctile.second.getNrOfTiles(), 10);
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();

            auto temp = desc[desctile.first].str().size();
            if (desctile.first == genie::core::AnnotDesc::SEQUENCEID) {
                EXPECT_EQ(temp, defaultTileSize * 2) << "descr: " << AnnotDescToString(desctile.first) << "," << i;
            }
            if (desctile.first == genie::core::AnnotDesc::STARTPOS) {
                EXPECT_EQ(temp, defaultTileSize * 8) << "descr: " << AnnotDescToString(desctile.first) << "," << i;
            }
            if (desctile.first == genie::core::AnnotDesc::DEPTH ||
                desctile.first == genie::core::AnnotDesc::MAPQUALITY) {
                EXPECT_EQ(temp, defaultTileSize * 4) << "descr: " << AnnotDescToString(desctile.first) << "," << i;
            }
            if (desctile.first == genie::core::AnnotDesc::STRAND) {
                EXPECT_EQ(temp, defaultTileSize / 4) << "descr: " << AnnotDescToString(desctile.first) << "," << i;
            }
            if (desctile.first == genie::core::AnnotDesc::LINKID || desctile.first == genie::core::AnnotDesc::FILTER) {
                EXPECT_EQ(temp, defaultTileSize) << "descr: " << AnnotDescToString(desctile.first) << "," << i;
            }
        }

        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attr;
        for (auto& attrtile : tile_attributeStream) {
            ASSERT_GE(attrtile.second.getNrOfTiles(), 10);
            attr[attrtile.first] = attrtile.second.getTypedTile(i);

            if (info[attrtile.first].getAttributeType() != genie::core::DataType::STRING) {
                auto datastreamSize = attr[attrtile.first].getDataStream().str().size();
                genie::core::ArrayType arraytpe;
                auto elemSize = arraytpe.getDefaultBitsize(info[attrtile.first].getAttributeType());
                auto nrOfArrays = info[attrtile.first].getArrayLength();

                auto expTileSize = datastreamSize * 8 / (nrOfArrays * elemSize);
                EXPECT_EQ(defaultTileSize, expTileSize);
            }
        }

        genie::variant_site::AccessUnitComposer accessUnit;
        accessUnit.setCompressors(compressors);
        accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit.at(i), AG_class, AT_ID,
                                 rowIndex);
        rowIndex += defaultTileSize;
    }

    std::ofstream testfile;
    std::string difName = "_moretiles_without";
    testfile.open(filepath + difName + ".bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filepath + difName + ".txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    APS_dataUnit.write(testwriter);
    APS_dataUnit.write(txtwriter);

    for (auto& aau : annotationAccessUnit) {
        genie::core::record::data_unit::Record AAU_dataUnit(aau);
        AAU_dataUnit.write(testwriter);
        AAU_dataUnit.write(txtwriter);
    }
    testfile.close();
    txtfile.close();
}
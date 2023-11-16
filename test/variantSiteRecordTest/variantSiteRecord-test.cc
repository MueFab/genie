#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/arrayType.h"
#include "genie/util/string-helpers.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/variantsite/VariantSiteParser.h"

#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/util/bitreader.h"

class VariantSiteRecordTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    VariantSiteRecordTests() = default;

    ~VariantSiteRecordTests() override = default;

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
        do {
            genie::core::record::variant_site::Record variant_site_record(reader);
            variant_site_record.write(outputfile);
        } while (inputfile.peek() != EOF);
        inputfile.close();
        outputfile.close();
    }
}
TEST_F(VariantSiteRecordTests, fixedValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint64_t variant_index = 1;
    uint16_t seq_ID = 1;
    uint64_t pos = 1;
    uint8_t strand = 1;
    uint8_t ID_len = 2;
    std::string ID = "UH";
    uint8_t description_len = 3;
    std::string description = "Poe";
    uint32_t ref_len = 1;
    std::string ref = "K";
    uint8_t alt_count = 5;

    std::vector<uint32_t> alt_len{3, 2, 5, 1, 2};
    std::vector<std::string> altern{"TCC", "AG", "TCGNT", "G", "NN"};

    uint32_t depth = 1;
    uint32_t seq_qual = 13241;
    uint32_t map_qual = 0;
    uint32_t map_num_qual_0 = 1313;
    uint8_t filters_len = 0;
    std::string filters = "";
    uint8_t info_count = 3;
    std::vector<genie::core::record::variant_site::Info_tag> info_tag;
    genie::core::record::variant_site::Info_tag infoTag;
    infoTag.info_array_len = 1;
    infoTag.info_tag = "AA";
    infoTag.info_tag_len = 2;
    infoTag.info_type = genie::core::DataType::CHAR;  // char
    infoTag.infoValue.resize(infoTag.info_array_len);
    infoTag.infoValue[0].push_back(63);
    info_tag.push_back(infoTag);

    infoTag.info_tag = "CD";
    infoTag.info_type = genie::core::DataType::UINT16;  // unsigned int
    infoTag.infoValue[0][0] = 24;
    infoTag.infoValue[0].push_back(0);
    info_tag.push_back(infoTag);

    infoTag.info_tag = "EF";
    infoTag.info_type = genie::core::DataType::STRING;  // string
    infoTag.infoValue[0][1] = 'A';
    infoTag.infoValue[0][1] = 'B';
    infoTag.infoValue[0].push_back('C');
    info_tag.push_back(infoTag);

    uint8_t linked_record = 0;
    uint8_t link_name_len = 0;
    std::string link_name = "";
    uint8_t reference_box_ID = 0;
    genie::core::record::variant_site::Record variant_site_record(
        variant_index, seq_ID, pos, strand, ID_len, ID, description_len, description, ref_len, ref, alt_count, alt_len,
        altern, depth, seq_qual, map_qual, map_num_qual_0, filters_len, filters, info_count, info_tag, linked_record,
        link_name_len, link_name, reference_box_ID);

    std::stringstream output;
    genie::core::Writer writer(&output);
    variant_site_record.write(writer);
    genie::util::BitReader reader(output);
    genie::core::record::variant_site::Record variant_site_record_check(reader);
    std::stringstream checkOut;
    genie::core::Writer writerCheck(&checkOut);
    variant_site_record_check.write(writerCheck);

    EXPECT_EQ(variant_site_record.getAlt(), variant_site_record_check.getAlt());
    EXPECT_EQ(variant_site_record.getInfoCount(), variant_site_record_check.getInfoCount());
    for (auto i = 0; i < variant_site_record.getInfoCount(); ++i) {
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_type, variant_site_record_check.getInfoTag()[i].info_type);
        EXPECT_EQ(variant_site_record.getInfoTag()[i].infoValue, variant_site_record_check.getInfoTag()[i].infoValue)
            << " index: " << std::to_string(i) << "\n";
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_array_len,
                  variant_site_record_check.getInfoTag()[i].info_array_len);
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_tag, variant_site_record_check.getInfoTag()[i].info_tag);
    }
    EXPECT_EQ(output.str(), checkOut.str());
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

TEST_F(VariantSiteRecordTests, readFileRunParser) {  // NOLINT(cert-err58-cpp)

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

    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, 0);
    ASSERT_TRUE(inputfile.is_open());
    if (inputfile.is_open()) {
        EXPECT_GE(parser.getNumberOfRows(), ExpectedNumberOfrows);
        inputfile.close();
    }

    genie::variant_site::ParameterSetComposer encodeParameters;

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
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descrList, info, parser.getNumberOfRows());

    //----------------------------------------------------//
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;
    genie::variant_site::AccessUnitComposer accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit, AG_class, AT_ID);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::string name = filepath + filename;

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

TEST_F(VariantSiteRecordTests, twotile) {  // NOLINT(cert-err58-cpp)
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
    std::ifstream inputfile;
    inputfile.open(filepath, std::ios::in | std::ios::binary);
    uint64_t defaultTileSize = 5000;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);
    if (inputfile.is_open()) inputfile.close();
    EXPECT_EQ(parser.getNrOfTiles(), 2);


    auto info = parser.getAttributes().getInfo();
    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    genie::variant_site::ParameterSetComposer encodeParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descrList, info, defaultTileSize);

    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

    genie::variant_site::AccessUnitComposer accessUnit;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit(parser.getNrOfTiles());
    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<genie::core::AnnotDesc, std::stringstream> desc;
        for (auto& desctile : tile_descriptorStream) {
            ASSERT_EQ(desctile.second.getNrOfTiles(), 2);
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();
        }

        std::map<std::string, std::stringstream> attr;
        for (auto& attrtile : tile_attributeStream) {
            ASSERT_EQ(attrtile.second.getNrOfTiles(), 2);
            attr[attrtile.first] << attrtile.second.getTile(i).rdbuf();
        }
        accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit.at(i), AG_class, AT_ID);
    }

    std::ofstream testfile;
    testfile.open(filepath + "2tiles.bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filepath + "2tiles.txt", std::ios::out);
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
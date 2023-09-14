#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/arrayType.h"
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

#define COMPRESSED true

TEST_F(VariantSiteRecordTests, readFileRunParser) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    using DescriptorID = genie::core::AnnotDesc;
    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.10000";
    std::ifstream inputfile;
    inputfile.open(path + filename + ".site", std::ios::in | std::ios::binary);

    filename = "ALL.chrX.phase1_release_v3.20101123.snps_indels_svs.genotypes.vcf.infotags.json";
    std::ifstream infoFieldsFile;
    infoFieldsFile.open(path + filename, std::ios::in);
    std::stringstream infoFields;
    if (infoFieldsFile.is_open()) {
        infoFields << infoFieldsFile.rdbuf();
        infoFieldsFile.close();
    }

    std::map<DescriptorID, std::stringstream> descriptorStream;
    std::map<std::string, genie::core::record::variant_site::AttributeData> attributesInfo;
    std::map<std::string, std::stringstream> attributeStream;
    genie::variant_site::VariantSiteParser parser(inputfile, descriptorStream, attributesInfo, attributeStream, infoFields);
    ASSERT_TRUE(inputfile.is_open());
    if (inputfile.is_open()) {
        EXPECT_GE(descriptorStream[DescriptorID::ALTERN].str().size(), 52);
        EXPECT_EQ(parser.getNumberOfRows(), 10000);
        inputfile.close();
    }

    EXPECT_EQ(attributeStream.size(), 22);
    for (auto it = attributeStream.begin(); it != attributeStream.end(); ++it)
        EXPECT_GE(it->second.str().size(), 10000) << it->first;

    genie::variant_site::ParameterSetComposer encodeParameters;

    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descriptorStream, attributesInfo);

    //----------------------------------------------------//

    genie::variant_site::AccessUnitComposer accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    accessUnit.setAccessUnit(descriptorStream, attributeStream, attributesInfo, annotationParameterSet, annotationAccessUnit);
    
        
    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::string subname = "Compressed";
    std::string name = "TestFiles/Complete" + subname;

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

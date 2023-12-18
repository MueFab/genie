#include <gtest/gtest.h>
#include <fstream>
//#include <iostream>
#include <sstream>

#include "genie//core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie//core/record/annotation_parameter_set/TileStructure.h"
#include "genie//core/record/annotation_parameter_set/record.h"

#include "genie//core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie//core/record/annotation_parameter_set/AttributeParameterSet.h"
#include "genie//core/record/annotation_parameter_set/CompressorParameterSet.h"

#include "genie//core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie//core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie//core/record/annotation_parameter_set/ContactMatrixParameters.h"
#include "genie//core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"

#include "genie//core/record/annotation_access_unit/AnnotationAccessUnitHeader.h"
#include "genie//core/record/annotation_access_unit/block.h"
#include "genie//core/record/annotation_access_unit/record.h"

#include "codecs/include/mpegg-codecs.h"

#define GENERATE_TEST_FILES false

class MpeggBSCTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    MpeggBSCTests() = default;

    ~MpeggBSCTests() override = default;

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

TEST_F(MpeggBSCTests, testPayload) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::ATTRIBUTE;
    uint8_t num_chrs = 1;

    std::string FileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::ifstream infile(FileName, std::ios::binary | std::ios::in);
    if (infile.fail()) {
        std::cout << "failed to open: " << FileName << std::endl;
    }
    std::stringstream generic_payload;
    if (infile.is_open()) {
        infile.seekg(0, infile.end);
        int length = static_cast<int>(infile.tellg());
        infile.seekg(0, infile.beg);
        EXPECT_GT(length, 0);

        generic_payload << infile.rdbuf();
        infile.close();
    }

    uint8_t readByte;
    std::vector<uint8_t> payloadVector;
    while (generic_payload >> readByte) payloadVector.push_back(readByte);

    uint16_t block_payload_size = static_cast<uint16_t>(generic_payload.str().size());

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptor_ID, num_chrs, 
        block_payload_size, payloadVector);

    std::stringstream outputData;
    genie::core::Writer writer(&outputData);
    block_payload.write(writer);
    EXPECT_GE(outputData.str().size(), block_payload_size);
}

TEST_F(MpeggBSCTests, testBlock) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::ATTRIBUTE;
    uint8_t num_chrs = 1;

    std::string FileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::ifstream infile(FileName, std::ios::binary | std::ios::in);
    if (infile.fail()) {
        std::cout << "failed to open: " << FileName << std::endl;
    }
    std::stringstream generic_payload;
    if (infile.is_open()) {
        infile.seekg(0, infile.end);
        int length = static_cast<int>(infile.tellg());
        infile.seekg(0, infile.beg);
        EXPECT_GT(length, 0);

        generic_payload << infile.rdbuf();
        infile.close();
    }
    std::vector<uint8_t> payloadvector;
    uint8_t readByte;
    while (generic_payload >> readByte) payloadvector.push_back(readByte);

    uint16_t block_payload_size = static_cast<uint16_t>(generic_payload.str().size());

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptor_ID, num_chrs, 
        block_payload_size, payloadvector);
    genie::core::record::annotation_access_unit::BlockHeader blockHeader(false, descriptor_ID, 1, false,
                                                                         block_payload_size);
    genie::core::record::annotation_access_unit::Block block(blockHeader, block_payload, num_chrs);

    std::stringstream outputData;
    genie::core::Writer writer(&outputData);
    block.write(writer);
    EXPECT_GE(outputData.str().size(), block_payload_size);
}

TEST_F(MpeggBSCTests, testCode) {  // NOLINT(cert-err58-cpp)
                                   // The rule of thumb is to use EXPECT_* when you want the test to continue
                                   // to reveal more errors after the assertion failure, and use ASSERT_*
                                   // when continuing after failure doesn't make sense.
    std::string inputFileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::string OutputFileName = "TestFiles/testCode.mgb";

    std::ifstream infile;
    infile.open(inputFileName, std::ios::binary);

    std::stringstream wholeTestFile;
    std::stringstream compressedData;
    if (infile.is_open()) {
        wholeTestFile << infile.rdbuf();
        infile.close();
    }

    unsigned char *dest = NULL;
    size_t destLen = 0;
    size_t srcLen = wholeTestFile.str().size();
//    std::cout << "source length: " << std::to_string(srcLen) << std::endl;

    mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)wholeTestFile.str().c_str(), srcLen,
                       MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                       MPEGG_BSC_CODER_QLFC_STATIC);
    // for (auto i = 0; i < destLen; ++i) compressedData << dest[i];

    compressedData.write((const char *)dest, destLen);

    auto compressed_dataSize = compressedData.str().size();
    std::cout << "compressedData length: " << std::to_string(compressed_dataSize) << std::endl;
}


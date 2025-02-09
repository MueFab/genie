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
#include "genie/util/string_helpers.h"

#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/variantsite/accessunit_composer.h"
#include "genie/variantsite/parameterset_composer.h"
#include "genie/variantsite/variantsite_parser.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/util/bit_reader.h"

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
    genie::util::Rtrim(result);

    return result;
}

}  // namespace util_tests


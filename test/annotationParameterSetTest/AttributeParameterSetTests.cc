
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "genie/core/writer.h"
#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/AttributeParameterSet.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class AttributeParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AttributeParameterSetTests() = default;

    ~AttributeParameterSetTests() override = default;

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
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(AttributeParameterSetTests, AttributeParameterSetValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint16_t attribute_ID = 1;
    uint8_t attribute_name_len = 5;
    std::string attribute_name = "12345";
    genie::core::DataType attribute_type = genie::core::DataType::CHAR;
    uint8_t attribute_num_array_dims = 5;
    std::vector<uint8_t> attribute_array_dims = {1, 2, 3, 4, 5};

    std::vector<uint8_t> attribute_default_val = {9, 8, 7, 6, 5};
    bool attribute_miss_val_flag = true;
    bool attribute_miss_default_flag = false;
    std::vector<uint8_t> attribute_miss_val = {4, 3, 2, 1, 0};

    std::string attribute_miss_str = "qwerty";
    uint8_t compressor_ID = 4;

    uint8_t n_steps_with_dependencies = 3;
    std::vector<uint8_t> dependency_step_ID = {7, 5, 3};
    std::vector<uint8_t> n_dependencies = {2, 2, 2};
    std::vector<std::vector<uint8_t>> dependency_var_ID{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {0, 0}};
    std::vector<std::vector<bool>> dependency_is_attribute{
        {true, true}, {false, false}, {false, true}, {true, false}, {true, true}};
    std::vector<std::vector<uint16_t>> dependency_ID{{3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}};

    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet(
        attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
        attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
        attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
        n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);

    EXPECT_EQ(attributeParameterSet.getCompressorID(), 4);

    std::stringstream InOut;
    genie::core::Writer strwriter(&InOut);
    attributeParameterSet.write(strwriter);

        genie::core::Writer writeSize;
    auto size = attributeParameterSet.getSize(writeSize);
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);
}

TEST_F(AttributeParameterSetTests, AttributeParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomattributeParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSetCheck;
    attributeParameterSet = randomattributeParameterSet.randomAttributeParameterSet();

    std::stringstream InOut;
    genie::core::Writer strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    attributeParameterSet.write(strwriter);
    strwriter.flush();
    attributeParameterSetCheck.read(strreader);
    std::stringstream TestOut;
    genie::core::Writer teststrwriter(&TestOut);
    attributeParameterSetCheck.write(teststrwriter);
    teststrwriter.flush();

    EXPECT_EQ(InOut.str(), TestOut.str());

        genie::core::Writer writeSize;
    auto size = attributeParameterSet.getSize(writeSize);
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(attributeParameterSet.getAttriubuteID(), attributeParameterSetCheck.getAttriubuteID());
    EXPECT_EQ(attributeParameterSet.getAttributeName(), attributeParameterSetCheck.getAttributeName());
    EXPECT_EQ(attributeParameterSet.getAttributeType(), attributeParameterSetCheck.getAttributeType());
    EXPECT_EQ(attributeParameterSet.getAttributeNumberOFArrayDims(),
              attributeParameterSetCheck.getAttributeNumberOFArrayDims());
    EXPECT_EQ(attributeParameterSet.getAttributeArrayDims(), attributeParameterSetCheck.getAttributeArrayDims());
    EXPECT_EQ(attributeParameterSet.getAttributeDefaultValue(), attributeParameterSetCheck.getAttributeDefaultValue());
    EXPECT_EQ(attributeParameterSet.isAttributeMissedValue(), attributeParameterSetCheck.isAttributeMissedValue());
    EXPECT_EQ(attributeParameterSet.getAttributeMissedValues(), attributeParameterSetCheck.getAttributeMissedValues());
    EXPECT_EQ(attributeParameterSet.getAttributeMissedString(), attributeParameterSetCheck.getAttributeMissedString());
    EXPECT_EQ(attributeParameterSet.getCompressorID(), attributeParameterSetCheck.getCompressorID());
    EXPECT_EQ(attributeParameterSet.areDependeciesAttributes(), attributeParameterSetCheck.areDependeciesAttributes());
    EXPECT_EQ(attributeParameterSet.getAttributeNameLength(), attributeParameterSetCheck.getAttributeNameLength());

#if GENERATE_TEST_FILES

    std::string name = "TestFiles/AttributeParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::core::Writer writer(&outputfile);
        attributeParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtWriter(&txtfile, true);
        attributeParameterSet.write(txtWriter);
        txtfile.close();
    }
#endif
}

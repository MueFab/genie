#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "genie/core/record/annotation_access_unit/BlockPayloadStream.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

class payloadStreamTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    payloadStreamTests() = default;

    ~payloadStreamTests() override = default;

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

TEST_F(payloadStreamTests, DISABLED_testcopy) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    std::stringstream testout;
    std::stringstream testout2;
    std::stringstream testout3;
    genie::util::BitWriter test2Writer(&testout2);
    genie::util::BitWriter testWriter(&testout);
    genie::util::BitWriter test3Writer(&testout3);

    genie::core::record::annotation_parameter_set::DescriptorID descriptorID =
        genie::core::record::annotation_parameter_set::DescriptorID::ATTRIBUTE;

    std::stringstream generic_payload;
    for (uint8_t i = 0; i < 200; ++i) {
        generic_payload << i;
    }
    uint32_t block_payload_size = static_cast<uint32_t>(generic_payload.str().size());

    genie::core::record::annotation_access_unit::BlockPayloadStream payload(descriptorID, block_payload_size,
                                                                            generic_payload);

    payload.write(testWriter);
    genie::core::record::annotation_access_unit::BlockPayloadStream payload2;
    payload2 = payload;
    genie::core::record::annotation_access_unit::BlockPayloadStream payload3(payload2);
    payload2.write(test2Writer);
    payload3.write(test3Writer);



      EXPECT_EQ(generic_payload.str(), testout.str());
      EXPECT_EQ(generic_payload.str(), testout2.str());
      EXPECT_EQ(generic_payload.str(), testout3.str());
}
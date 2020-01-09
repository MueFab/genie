#include <gtest/gtest.h>
#include <coding/gabac-compressor.h>
#include <util/drain.h>
#include "helpers.h"
#include <iostream>



TEST(GabacCompressor, NonEmpty) {  // NOLINT(cert-err58-cpp)

    class DummyModule : public Drain<std::unique_ptr<BlockPayloadSet>> {
        public:
            void flowIn(std::unique_ptr<BlockPayloadSet>&& t, size_t id) override {
                /* This is where we catch the result of GabacCompressor. Collect any information about results here */
            }

            void dryIn() override {
                /* This should be called in the end after the last result ended up in flowIn */
            }

            int veryImportantExampleInformation() { return 42; } /* Dummy information needed for tests */
            bool veryImportantExampleTest() { return 1 + 1 == 2; } /* Dummy check */
    };

    GabacCompressor compressor;
    DummyModule dummy;
    compressor.setDrain(&dummy);

    size_t numRecs = 50;
    auto raw_aus = util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>(), numRecs);

//    for (size_t i = 0; i < 50; ++i) {
//        raw_aus->get(MpeggRawAu::GenomicDescriptor::RLEN, 0).push(i) // Append some data for one descriptor
//        // See mpegg-raw-au.cc/getDescriptorProperties() for all valid descriptors and how many subsequences there are for each descriptor (here we are only populating sequence 0)
//        // Maybe even use different, random data for each run? You should vary test data a little bit to cover more cases and also use different descriptor streams
//    }
//
//    // Execute
//    compressor.flowIn(std::move(raw_aus), 0);
//    compressor.dryIn();
//
//
//    EXPECT_NE(dummy.veryImportantExampleInformation(), 42);
//    ASSERT_FALSE(dummy.veryImportantExampleTest());
}
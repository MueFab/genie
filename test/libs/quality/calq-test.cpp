#include <gtest/gtest.h>
#include <iostream>
#include "genie/core/record/alignment-box.h"
#include "genie/core/record/record.h"
#include "genie/quality/calq/local-reference.h"

using Record = genie::core::record::Record;
using Segment = genie::core::record::Segment;

TEST(CalqTest, local_reference) {
    auto pileup = genie::quality::calq::LocalReference(1024);

    // usually done in addRead, but i dont want to craft records
    // first "record"
    pileup.sequences.emplace_back();
    pileup.sequence_positions.emplace_back();
    pileup.qualities.emplace_back();

    pileup.addSingleRead("AAAA", "aaaa", "4=", 0);
    pileup.addSingleRead("GGGG", "gggg", "4=", 10);

    // second "record"
    pileup.sequences.emplace_back();
    pileup.sequence_positions.emplace_back();
    pileup.qualities.emplace_back();

    pileup.addSingleRead("CCCC", "cccc", "4=", 2);
    pileup.addSingleRead("TTTTTTTT", "tttttttt", "8=", 8);

    // third "record"
    pileup.sequences.emplace_back();
    pileup.sequence_positions.emplace_back();
    pileup.qualities.emplace_back();

    pileup.addSingleRead("AAAA", "aaaa", "4=", 10);
    pileup.addSingleRead("CCCC", "cccc", "4=", 11);

    std::string seq, qual;

    std::tie(seq, qual) = pileup.getPileup(0);
    ASSERT_EQ(seq, "A");
    ASSERT_EQ(qual, "a");

    std::tie(seq, qual) = pileup.getPileup(2);
    ASSERT_EQ(seq, "AC");
    ASSERT_EQ(qual, "ac");

    std::tie(seq, qual) = pileup.getPileup(4);
    ASSERT_EQ(seq, "C");
    ASSERT_EQ(qual, "c");

    std::tie(seq, qual) = pileup.getPileup(6);
    ASSERT_EQ(seq, "");
    ASSERT_EQ(qual, "");

    std::tie(seq, qual) = pileup.getPileup(11);
    ASSERT_EQ(seq, "GTAC");
    ASSERT_EQ(qual, "gtac");

    std::tie(seq, qual) = pileup.getPileup(15);
    ASSERT_EQ(seq, "T");
    ASSERT_EQ(qual, "t");

    EXPECT_ANY_THROW(pileup.getPileup(16));
}

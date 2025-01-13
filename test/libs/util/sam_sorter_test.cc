

#include <gtest/gtest.h>
#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"
#include "genie/format/sam/sam_to_mgrec/sam_record.h"

TEST(SamSorterTest, easyExample) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter = genie::format::sam::sam_to_mgrec::SamSorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 5;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 5;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;
    result.push_back(records);
    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, complexExample) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 1;
    record.mate_pos = 99;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 5;
    record.mate_pos = 201;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 13;
    record.mate_pos = 13;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 13;
    record.mate_pos = 13;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 25;
    record.mate_pos = 67;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair6";
    record.pos = 39;
    record.mate_pos = 67;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair6";
    record.pos = 67;
    record.mate_pos = 39;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 99;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 201;
    record.mate_pos = 5;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair7";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(1));
    tmp.push_back(records.at(8));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(3));
    tmp.push_back(records.at(4));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(5));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(10));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, waitingDistance) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 123;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 4;
    record.mate_pos = 105;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 56;
    record.mate_pos = 57;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 57;
    record.mate_pos = 56;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 93;
    record.mate_pos = 145;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 102;
    record.mate_pos = 256;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 105;
    record.mate_pos = 4;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 123;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 145;
    record.mate_pos = 93;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 256;
    record.mate_pos = 102;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(1));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(4));
    tmp.push_back(records.at(8));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(5));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, unalignedPairWithZero) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 0;
    record.mate_pos = 0;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    tmp.push_back(records.at(1));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(4));
    tmp.push_back(records.at(5));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(8));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, unalignedPairWithMaxValue) {
     genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = std::numeric_limits<uint32_t>::max();
    record.mate_pos = std::numeric_limits<uint32_t>::max();
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    tmp.push_back(records.at(1));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(4));
    tmp.push_back(records.at(5));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(8));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, alignedPair) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 66;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 13;
    record.mate_pos = 51;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 26;
    record.mate_pos = 82;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 51;
    record.mate_pos = 13;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 66;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 79;
    record.mate_pos = 125;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 82;
    record.mate_pos = 26;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 111;
    record.mate_pos = 129;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 125;
    record.mate_pos = 79;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 129;
    record.mate_pos = 111;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    tmp.push_back(records.at(4));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(1));
    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    tmp.push_back(records.at(6));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(5));
    tmp.push_back(records.at(8));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(7));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, singleRecordWithoutMate) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 123;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 23;
    record.mate_pos = 89;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 56;
    record.mate_pos = 57;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 67;
    record.mate_pos = 25;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 81;
    record.mate_pos = 504;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair6";
    record.pos = 93;
    record.mate_pos = 145;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair7";
    record.pos = 102;
    record.mate_pos = 156;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair8";
    record.pos = 124;
    record.mate_pos = 2;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair6";
    record.pos = 145;
    record.mate_pos = 93;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair7";
    record.pos = 156;
    record.mate_pos = 102;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(1));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(4));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(5));
    tmp.push_back(records.at(8));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, samePositionAsMate) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 56;
    record.mate_pos = 56;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 56;
    record.mate_pos = 56;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 89;
    record.mate_pos = 89;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 89;
    record.mate_pos = 89;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 102;
    record.mate_pos = 156;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 156;
    record.mate_pos = 102;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 178;
    record.mate_pos = 178;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 178;
    record.mate_pos = 178;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    tmp.push_back(records.at(1));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(2));
    tmp.push_back(records.at(3));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(4));
    tmp.push_back(records.at(5));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(8));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);
}

TEST(SamSorterTest, positionMultipleTimes) {
    genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
    genie::format::sam::sam_to_mgrec::SamRecord record;
    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
    std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

    record.qname = "pair1";
    record.pos = 1;
    record.mate_pos = 23;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 23;
    record.mate_pos = 89;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair1";
    record.pos = 23;
    record.mate_pos = 1;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 57;
    record.mate_pos = 145;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair2";
    record.pos = 89;
    record.mate_pos = 23;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 93;
    record.mate_pos = 145;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 102;
    record.mate_pos = 156;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair4";
    record.pos = 145;
    record.mate_pos = 93;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair3";
    record.pos = 145;
    record.mate_pos = 57;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    record.qname = "pair5";
    record.pos = 156;
    record.mate_pos = 102;
    sam_sorter.addSamRead(record);
    records.push_back(record);

    sam_sorter.endFile();

    std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

    tmp.push_back(records.at(0));
    tmp.push_back(records.at(2));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(1));
    tmp.push_back(records.at(4));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(3));
    tmp.push_back(records.at(8));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(5));
    tmp.push_back(records.at(7));
    result.push_back(tmp);
    tmp.clear();

    tmp.push_back(records.at(6));
    tmp.push_back(records.at(9));
    result.push_back(tmp);
    tmp.clear();

    EXPECT_EQ(sam_sorter.getQueries(), result);


}
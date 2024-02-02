/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include "genie/core/record/contact/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, one_rec_no_norm) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::ContactRecord> recs;

    while (bitreader.isGood()){
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    auto& rec = recs.front();

    ASSERT_EQ(rec.getSampleID(), 0);
    ASSERT_EQ(rec.getSampleName(), "Test01");
    ASSERT_EQ(rec.getChr1ID(), 21);
    ASSERT_EQ(rec.getChr1Name(), "22");
    ASSERT_EQ(rec.getChr2ID(), 21);
    ASSERT_EQ(rec.getChr2Name(), "22");
    ASSERT_EQ(rec.getNumCounts(), 9812);
    ASSERT_EQ(rec.getNumNormCounts(), 0);
    ASSERT_EQ(rec.getStartPos1().front(), 16000000);
    ASSERT_EQ(rec.getStartPos1().back(), 51000000);
    ASSERT_EQ(rec.getEndPos1().front(), 16250000);
    ASSERT_EQ(rec.getEndPos1().back(), 51250000);
    ASSERT_EQ(rec.getStartPos2().front(), 16000000);
    ASSERT_EQ(rec.getStartPos2().back(), 51000000);
    ASSERT_EQ(rec.getEndPos2().front(), 16250000);
    ASSERT_EQ(rec.getEndPos2().back(), 51250000);
    ASSERT_EQ(rec.getCounts().front(), 16);
    ASSERT_EQ(rec.getCounts().back(), 68156);
}

TEST(ContactRecord, three_rec_no_norm) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-250000-three_recs.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::ifstream reader(filepath, std::ios::binary);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::ContactRecord> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    // Test first record
    {
        auto& rec = recs.front();
        ASSERT_EQ(rec.getSampleID(), 0);
        ASSERT_EQ(rec.getSampleName(), "Test01");
        ASSERT_EQ(rec.getChr1ID(), 19);
        ASSERT_EQ(rec.getChr1Name(), "20");
        ASSERT_EQ(rec.getChr2ID(), 19);
        ASSERT_EQ(rec.getChr2Name(), "20");
//        ASSERT_EQ(rec.getNumCounts(), 9812);
//        ASSERT_EQ(rec.getNumNormCounts(), 0);
//        ASSERT_EQ(rec.getStartPos1().front(), 16000000);
//        ASSERT_EQ(rec.getStartPos1().back(), 51000000);
//        ASSERT_EQ(rec.getEndPos1().front(), 16250000);
//        ASSERT_EQ(rec.getEndPos1().back(), 51250000);
//        ASSERT_EQ(rec.getStartPos2().front(), 16000000);
//        ASSERT_EQ(rec.getStartPos2().back(), 51000000);
//        ASSERT_EQ(rec.getEndPos2().front(), 16250000);
//        ASSERT_EQ(rec.getEndPos2().back(), 51250000);
//        ASSERT_EQ(rec.getCounts().front(), 16);
//        ASSERT_EQ(rec.getCounts().back(), 68156);
    }

    // Test last record
    {
        auto& rec = recs.back();
        ASSERT_EQ(rec.getSampleID(), 0);
        ASSERT_EQ(rec.getSampleName(), "Test01");
        ASSERT_EQ(rec.getChr1ID(), 21);
        ASSERT_EQ(rec.getChr1Name(), "22");
        ASSERT_EQ(rec.getChr2ID(), 21);
        ASSERT_EQ(rec.getChr2Name(), "22");
        ASSERT_EQ(rec.getNumCounts(), 9812);
        ASSERT_EQ(rec.getNumNormCounts(), 0);
        ASSERT_EQ(rec.getStartPos1().front(), 16000000);
        ASSERT_EQ(rec.getStartPos1().back(), 51000000);
        ASSERT_EQ(rec.getEndPos1().front(), 16250000);
        ASSERT_EQ(rec.getEndPos1().back(), 51250000);
        ASSERT_EQ(rec.getStartPos2().front(), 16000000);
        ASSERT_EQ(rec.getStartPos2().back(), 51000000);
        ASSERT_EQ(rec.getEndPos2().front(), 16250000);
        ASSERT_EQ(rec.getEndPos2().back(), 51250000);
        ASSERT_EQ(rec.getCounts().front(), 16);
        ASSERT_EQ(rec.getCounts().back(), 68156);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
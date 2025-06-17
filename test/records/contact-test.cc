/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <cmath>
#include <fstream>
#include "genie/core/contact_record/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "helpers.h"

void check_row_col_uniqueness(
    genie::core::record::ContactRecord& rec
){
    size_t num_entries = rec.GetNumEntries();

    auto& starts1 = rec.GetStartPos1();
    auto& starts2 = rec.GetStartPos2();
    auto& ends1 = rec.GetEndPos1();
    auto& ends2 = rec.GetEndPos2();
    auto& counts = rec.GetCounts();

    std::map<std::pair<uint64_t, uint64_t>, uint32_t> sparse_mat1;
    std::map<std::pair<uint64_t, uint64_t>, uint32_t> sparse_mat2;

    for (auto i_entry = 0u; i_entry< num_entries; i_entry++){
        auto count = counts[i_entry];
        {
            auto row_id = starts1[i_entry];
            auto col_id = starts2[i_entry];
            auto row_col_id_pair = std::pair<uint64_t, uint64_t>(row_id, col_id);

            auto it = sparse_mat1.find(row_col_id_pair);
            ASSERT_EQ(it, sparse_mat1.end());
            sparse_mat1.emplace(row_col_id_pair, count);
        }

        {
            auto row_id = ends1[i_entry];
            auto col_id = ends2[i_entry];
            auto row_col_id_pair = std::pair<uint64_t, uint64_t>(row_id, col_id);

            auto it = sparse_mat2.find(row_col_id_pair);
            ASSERT_EQ(it, sparse_mat2.end());
            sparse_mat2.emplace(row_col_id_pair, count);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, IntraContactRecord_LR_Raw) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    auto& rec = recs.front();

    ASSERT_EQ(rec.GetSampleID(), 0u);
    ASSERT_EQ(rec.GetSampleName(), "Test01");
    ASSERT_EQ(rec.GetChr1ID(), 21u);
    ASSERT_EQ(rec.GetChr1Name(), "22");
    ASSERT_EQ(rec.GetChr2ID(), 21u);
    ASSERT_EQ(rec.GetChr2Name(), "22");
    ASSERT_EQ(rec.GetNumEntries(), 9812u);
    ASSERT_EQ(rec.GetNumNormCounts(), 0u);
    ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
    ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
    ASSERT_EQ(rec.GetEndPos1().front(), 16250000u);
    ASSERT_EQ(rec.GetEndPos1().back(), 51250000u);
    ASSERT_EQ(rec.GetStartPos2().front(), 16000000u);
    ASSERT_EQ(rec.GetStartPos2().back(), 51000000u);
    ASSERT_EQ(rec.GetEndPos2().front(), 16250000u);
    ASSERT_EQ(rec.GetEndPos2().back(), 51250000u);
    ASSERT_EQ(rec.GetCounts().front(), 16u);
    ASSERT_EQ(rec.GetCounts().back(), 68156u);

    {
        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, IntraContactRecord_LR_All) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-all-250000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    {
        auto& rec = recs.front();

        ASSERT_EQ(rec.GetSampleID(), 0u);
        ASSERT_EQ(rec.GetSampleName(), "Test01");
        ASSERT_EQ(rec.GetChr1ID(), 21u);
        ASSERT_EQ(rec.GetChr1Name(), "22");
        ASSERT_EQ(rec.GetChr2ID(), 21u);
        ASSERT_EQ(rec.GetChr2Name(), "22");
        ASSERT_EQ(rec.GetNumEntries(), 9623u);
        ASSERT_EQ(rec.GetNumNormCounts(), 7u);
        ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
        ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
        ASSERT_EQ(rec.GetEndPos1().front(), 16250000u);
        ASSERT_EQ(rec.GetEndPos1().back(), 51250000u);
        ASSERT_EQ(rec.GetStartPos2().front(), 16000000u);
        ASSERT_EQ(rec.GetStartPos2().back(), 51000000u);
        ASSERT_EQ(rec.GetEndPos2().front(), 16250000u);
        ASSERT_EQ(rec.GetEndPos2().back(), 51250000u);
        ASSERT_EQ(rec.GetCounts().front(), 16u);
        ASSERT_EQ(rec.GetCounts().back(), 68156u);
        ASSERT_EQ(rec.GetNormCountNames().front(), "GW_KR");
        ASSERT_EQ(rec.GetNormCountNames().back(), "VC_SQRT");
        ASSERT_TRUE(abs(rec.GetNormCounts()[0].front() - 0.002699L) < 1e-5L);
        ASSERT_TRUE(abs(rec.GetNormCounts()[0].back() - 70297.121260L) < 1e-5L);

        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, IntraContactRecord_HR_Raw) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    auto& rec = recs.front();

    ASSERT_EQ(rec.GetSampleID(), 0u);
    ASSERT_EQ(rec.GetSampleName(), "Test01");
    ASSERT_EQ(rec.GetChr1ID(), 21u);
    ASSERT_EQ(rec.GetChr1Name(), "22");
    ASSERT_EQ(rec.GetChr2ID(), 21u);
    ASSERT_EQ(rec.GetChr2Name(), "22");
    //TODO: Complete all with the correct values
    //    ASSERT_EQ(rec.GetNumEntries(), 9812u);
    //    ASSERT_EQ(rec.getNumNormCounts(), 0u);
    //    ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
    //    ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos1().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos1().back(), 51250000u);
    //    ASSERT_EQ(rec.getStartPos2().front(), 16000000u);
    //    ASSERT_EQ(rec.getStartPos2().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos2().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos2().back(), 51250000u);
    //    ASSERT_EQ(rec.getCounts().front(), 16u);
    //    ASSERT_EQ(rec.getCounts().back(), 68156u);

    {
        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, IntraContactRecord_HR_All) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.mcool-all-50000-21_21.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    auto& rec = recs.front();

    ASSERT_EQ(rec.GetSampleID(), 0u);
    ASSERT_EQ(rec.GetSampleName(), "Test01");
    ASSERT_EQ(rec.GetChr1ID(), 21u);
    ASSERT_EQ(rec.GetChr1Name(), "22");
    ASSERT_EQ(rec.GetChr2ID(), 21u);
    ASSERT_EQ(rec.GetChr2Name(), "22");
    //TODO: Complete all with the correct values
    //    ASSERT_EQ(rec.GetNumEntries(), 9812u);
    //    ASSERT_EQ(rec.getNumNormCounts(), 0u);
    //    ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
    //    ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos1().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos1().back(), 51250000u);
    //    ASSERT_EQ(rec.getStartPos2().front(), 16000000u);
    //    ASSERT_EQ(rec.getStartPos2().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos2().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos2().back(), 51250000u);
    //    ASSERT_EQ(rec.getCounts().front(), 16u);
    //    ASSERT_EQ(rec.getCounts().back(), 68156u);

    {
        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, InterContactRecord_LR_Raw) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-250000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    auto& rec = recs.front();

    ASSERT_EQ(rec.GetSampleID(), 0u);
    ASSERT_EQ(rec.GetSampleName(), "Test01");
    ASSERT_EQ(rec.GetChr1ID(), 21u);
    ASSERT_EQ(rec.GetChr1Name(), "22");
    ASSERT_EQ(rec.GetChr2ID(), 22u);
    ASSERT_EQ(rec.GetChr2Name(), "X");
    //TODO: Complete all with the correct values
//    ASSERT_EQ(rec.GetNumEntries(), 59740);
//    ASSERT_EQ(rec.getNumNormCounts(), 0u);
//    ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
//    ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
//    ASSERT_EQ(rec.getEndPos1().front(), 16250000u);
//    ASSERT_EQ(rec.getEndPos1().back(), 51250000u);
//    ASSERT_EQ(rec.getStartPos2().front(), 16000000u);
//    ASSERT_EQ(rec.getStartPos2().back(), 51000000u);
//    ASSERT_EQ(rec.getEndPos2().front(), 16250000u);
//    ASSERT_EQ(rec.getEndPos2().back(), 51250000u);
//    ASSERT_EQ(rec.getCounts().front(), 16u);
//    ASSERT_EQ(rec.getCounts().back(), 68156u);

    {
        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactRecord, InterContactRecord_HR_Raw) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-50000-21_22.cont";
    std::string filepath = gitRootDir + "/data/records/contact/" + filename;

    std::vector<genie::core::record::ContactRecord> recs;

    {
        std::ifstream reader(filepath, std::ios::binary);
        ASSERT_EQ(reader.fail(), false);
        genie::util::BitReader bitreader(reader);

        while (bitreader.IsStreamGood()){
            recs.emplace_back(bitreader);
        }

        // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
        recs.pop_back();
    }

    auto& rec = recs.front();

    ASSERT_EQ(rec.GetSampleID(), 0u);
    ASSERT_EQ(rec.GetSampleName(), "Test01");
    ASSERT_EQ(rec.GetChr1ID(), 21u);
    ASSERT_EQ(rec.GetChr1Name(), "22");
    ASSERT_EQ(rec.GetChr2ID(), 22u);
    ASSERT_EQ(rec.GetChr2Name(), "X");
    //TODO: Complete all with the correct values
    //    ASSERT_EQ(rec.GetNumEntries(), 59740);
    //    ASSERT_EQ(rec.getNumNormCounts(), 0u);
    //    ASSERT_EQ(rec.GetStartPos1().front(), 16000000u);
    //    ASSERT_EQ(rec.GetStartPos1().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos1().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos1().back(), 51250000u);
    //    ASSERT_EQ(rec.getStartPos2().front(), 16000000u);
    //    ASSERT_EQ(rec.getStartPos2().back(), 51000000u);
    //    ASSERT_EQ(rec.getEndPos2().front(), 16250000u);
    //    ASSERT_EQ(rec.getEndPos2().back(), 51250000u);
    //    ASSERT_EQ(rec.getCounts().front(), 16u);
    //    ASSERT_EQ(rec.getCounts().back(), 68156u);

    {
        std::stringstream ss;
        genie::util::BitWriter writer(&ss);

        rec.Write(writer);

        genie::util::BitReader bitreader(ss);
        auto recon_rec = genie::core::record::ContactRecord(bitreader);

        ASSERT_TRUE(rec == recon_rec);

        check_row_col_uniqueness(recon_rec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "genie/core/record/contact/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

namespace genie::core::record {

TEST(ContactRecord, RoundTrip) {
    uint16_t sample_ID = 1;
    std::string sample_name = "test_sample";
    uint32_t bin_size = 1000;
    uint8_t chr1_ID = 1;
    std::string chr1_name = "chr1";
    uint64_t chr1_length = 1000000;
    uint8_t chr2_ID = 2;
    std::string chr2_name = "chr2";
    uint64_t chr2_length = 2000000;
    std::vector<std::string> norm_count_names = {"norm1", "norm2"};
    std::vector<uint64_t> start_pos1 = {100, 200};
    std::vector<uint64_t> end_pos1 = {110, 210};
    std::vector<uint64_t> start_pos2 = {300, 400};
    std::vector<uint64_t> end_pos2 = {310, 410};
    std::vector<uint32_t> counts = {5, 10};

    ContactRecord orig_rec(
        sample_ID, std::string(sample_name), bin_size,
        chr1_ID, std::string(chr1_name), chr1_length,
        chr2_ID, std::string(chr2_name), chr2_length,
        std::vector<std::string>(norm_count_names),
        std::vector<uint64_t>(start_pos1),
        std::vector<uint64_t>(end_pos1),
        std::vector<uint64_t>(start_pos2),
        std::vector<uint64_t>(end_pos2),
        std::vector<uint32_t>(counts)
    );

    std::stringstream ss;
    {
        util::BitWriter writer(ss);
        orig_rec.Write(writer);
    }

    util::BitReader reader(ss);
    ContactRecord recon_rec(reader);

    EXPECT_EQ(recon_rec.GetSampleID(), sample_ID);
    EXPECT_EQ(recon_rec.GetSampleName(), sample_name);
    EXPECT_EQ(recon_rec.GetBinSize(), bin_size);
    EXPECT_EQ(recon_rec.GetChr1ID(), chr1_ID);
    EXPECT_EQ(recon_rec.GetChr1Name(), chr1_name);
    EXPECT_EQ(recon_rec.GetChr1Length(), chr1_length);
    EXPECT_EQ(recon_rec.GetChr2ID(), chr2_ID);
    EXPECT_EQ(recon_rec.GetChr2Name(), chr2_name);
    EXPECT_EQ(recon_rec.GetChr2Length(), chr2_length);
    EXPECT_EQ(recon_rec.GetNumEntries(), start_pos1.size());
    EXPECT_EQ(recon_rec.GetStartPos1(), start_pos1);
    EXPECT_EQ(recon_rec.GetEndPos1(), end_pos1);
    EXPECT_EQ(recon_rec.GetStartPos2(), start_pos2);
    EXPECT_EQ(recon_rec.GetEndPos2(), end_pos2);
    EXPECT_EQ(recon_rec.GetCounts(), counts);
}

} // namespace genie::core::record

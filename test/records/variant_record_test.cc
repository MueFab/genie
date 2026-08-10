/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "genie/core/record/site/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

namespace genie::core::record::site {

TEST(VariantSiteRecord, RoundTrip) {
    uint64_t variant_index = 1;
    uint16_t seq_id = 1;
    uint64_t pos = 1000;
    uint8_t strand = 0;
    std::string id = "rs123";
    uint8_t id_len = static_cast<uint8_t>(id.size());
    std::string description = "desc";
    uint8_t description_len = static_cast<uint8_t>(description.size());
    std::string ref = "A";
    uint32_t ref_len = static_cast<uint32_t>(ref.size());
    uint8_t alt_count = 1;
    std::vector<uint32_t> alt_len = {1};
    std::vector<std::string> altern = {"G"};
    uint32_t depth = 30;
    uint32_t seq_qual = 60;
    uint32_t map_qual = 60;
    uint32_t map_num_qual_0 = 0;
    std::string filters = "PASS";
    uint8_t filters_len = static_cast<uint8_t>(filters.size());
    uint8_t linked_record = 0;
    uint8_t link_name_len = 0;
    std::string link_name = "";
    uint8_t reference_box_ID = 0;

    Record orig_rec(
        variant_index, seq_id, pos, strand, id_len,
        id, description_len, description, ref_len,
        ref, alt_count, alt_len, altern, depth,
        seq_qual, map_qual, map_num_qual_0, filters_len,
        filters, linked_record, link_name_len, link_name,
        reference_box_ID
    );

    std::stringstream ss;
    {
        util::BitWriter writer(ss);
        orig_rec.Write(writer);
    }

    util::BitReader reader(ss);
    Record recon_rec(reader);

    EXPECT_EQ(recon_rec.GetVariantIndex(), variant_index);
    EXPECT_EQ(recon_rec.GetSeqId(), seq_id);
    EXPECT_EQ(recon_rec.GetPos(), pos);
    EXPECT_EQ(recon_rec.GetId(), id);
    EXPECT_EQ(recon_rec.GetRef(), ref);
    EXPECT_EQ(recon_rec.GetAlt(), altern);
}

} // namespace genie::core::record::variant_site

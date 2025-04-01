/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/genotype/amax_payload.h"
#include "genie/genotype/bin_mat_payload.h"
#include "helpers.h"

// -----------------------------------------------------------------------------

TEST(GenotypeStructure, RoundTrip_Structure_BinMatPayload){
    uint32_t ORIG_NCOLS = 23;
    uint32_t ORIG_NROWS = 5;

    size_t ORIG_PAYLOAD_LEN = 37;
    uint8_t ORIG_PAYLOAD[37] = {
        0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
        0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2
    };

    {
        auto ORIG_CODEC_ID = genie::core::AlgoID::JBIG;
        std::vector<uint8_t> payload(std::begin(ORIG_PAYLOAD), std::end(ORIG_PAYLOAD));

        genie::genotype::BinMatPayload bin_mat_payload(
            ORIG_CODEC_ID,
            std::move(payload),
            ORIG_NROWS,
            ORIG_NCOLS
        );

        ASSERT_EQ(ORIG_NROWS, bin_mat_payload.GetNRows());
        ASSERT_EQ(ORIG_NCOLS, bin_mat_payload.GetNCols());
        ASSERT_EQ(ORIG_PAYLOAD_LEN, bin_mat_payload.GetPayloadSize());
        ASSERT_EQ(ORIG_CODEC_ID, bin_mat_payload.GetCodecID());

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bit_writer = genie::util::BitWriter(&writer);
        bin_mat_payload.Write(bit_writer);

        auto payload_size = obj_payload.str().size();
        ASSERT_EQ(payload_size, bin_mat_payload.GetSize());

        std::istream& reader = obj_payload;
        auto bit_reader = genie::util::BitReader(reader);
        auto recon_obj = genie::genotype::BinMatPayload(
            bit_reader,
            payload_size,
            ORIG_CODEC_ID
        );

        ASSERT_EQ(ORIG_NROWS, recon_obj.GetNRows());
        ASSERT_EQ(ORIG_NCOLS, recon_obj.GetNCols());
        ASSERT_EQ(ORIG_PAYLOAD_LEN, recon_obj.GetPayloadSize());
        ASSERT_EQ(ORIG_CODEC_ID, recon_obj.GetCodecID());

        ASSERT_TRUE(bin_mat_payload == recon_obj);
    }

    {
      auto ORIG_CODEC_ID = genie::core::AlgoID::ZSTD;
      std::vector<uint8_t> payload(std::begin(ORIG_PAYLOAD), std::end(ORIG_PAYLOAD));

      genie::genotype::BinMatPayload bin_mat_payload(
          ORIG_CODEC_ID,
          std::move(payload),
          ORIG_NROWS,
          ORIG_NCOLS
      );

      ASSERT_EQ(ORIG_NROWS, bin_mat_payload.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, bin_mat_payload.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, bin_mat_payload.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, bin_mat_payload.GetCodecID());

      auto obj_payload = std::stringstream();
      std::ostream& writer = obj_payload;
      auto bit_writer = genie::util::BitWriter(&writer);
      bin_mat_payload.Write(bit_writer);

      auto payload_size = obj_payload.str().size();
      ASSERT_EQ(payload_size, bin_mat_payload.GetSize());

      std::istream& reader = obj_payload;
      auto bit_reader = genie::util::BitReader(reader);
      auto recon_obj = genie::genotype::BinMatPayload(
          bit_reader,
          payload_size,
          ORIG_CODEC_ID
      );

      ASSERT_EQ(ORIG_NROWS, recon_obj.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, recon_obj.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, recon_obj.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, recon_obj.GetCodecID());

      ASSERT_TRUE(bin_mat_payload == recon_obj);
    }

    {
      auto ORIG_CODEC_ID = genie::core::AlgoID::BSC;
      std::vector<uint8_t> payload(std::begin(ORIG_PAYLOAD), std::end(ORIG_PAYLOAD));

      genie::genotype::BinMatPayload bin_mat_payload(
          ORIG_CODEC_ID,
          std::move(payload),
          ORIG_NROWS,
          ORIG_NCOLS
      );

      ASSERT_EQ(ORIG_NROWS, bin_mat_payload.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, bin_mat_payload.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, bin_mat_payload.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, bin_mat_payload.GetCodecID());

      auto obj_payload = std::stringstream();
      std::ostream& writer = obj_payload;
      auto bit_writer = genie::util::BitWriter(&writer);
      bin_mat_payload.Write(bit_writer);

      auto payload_size = obj_payload.str().size();
      ASSERT_EQ(payload_size, bin_mat_payload.GetSize());

      std::istream& reader = obj_payload;
      auto bit_reader = genie::util::BitReader(reader);
      auto recon_obj = genie::genotype::BinMatPayload(
          bit_reader,
          payload_size,
          ORIG_CODEC_ID
      );

      ASSERT_EQ(ORIG_NROWS, recon_obj.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, recon_obj.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, recon_obj.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, recon_obj.GetCodecID());

      ASSERT_TRUE(bin_mat_payload == recon_obj);
    }

    {
      auto ORIG_CODEC_ID = genie::core::AlgoID::LZMA;
      std::vector<uint8_t> payload(std::begin(ORIG_PAYLOAD), std::end(ORIG_PAYLOAD));

      genie::genotype::BinMatPayload bin_mat_payload(
          ORIG_CODEC_ID,
          std::move(payload),
          ORIG_NROWS,
          ORIG_NCOLS
      );

      ASSERT_EQ(ORIG_NROWS, bin_mat_payload.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, bin_mat_payload.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, bin_mat_payload.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, bin_mat_payload.GetCodecID());

      auto obj_payload = std::stringstream();
      std::ostream& writer = obj_payload;
      auto bit_writer = genie::util::BitWriter(&writer);
      bin_mat_payload.Write(bit_writer);

      auto payload_size = obj_payload.str().size();
      ASSERT_EQ(payload_size, bin_mat_payload.GetSize());

      std::istream& reader = obj_payload;
      auto bit_reader = genie::util::BitReader(reader);
      auto recon_obj = genie::genotype::BinMatPayload(
          bit_reader,
          payload_size,
          ORIG_CODEC_ID
      );

      ASSERT_EQ(ORIG_NROWS, recon_obj.GetNRows());
      ASSERT_EQ(ORIG_NCOLS, recon_obj.GetNCols());
      ASSERT_EQ(ORIG_PAYLOAD_LEN, recon_obj.GetPayloadSize());
      ASSERT_EQ(ORIG_CODEC_ID, recon_obj.GetCodecID());

      ASSERT_TRUE(bin_mat_payload == recon_obj);
    }
}

// -----------------------------------------------------------------------------

TEST(GenotypeStructure, RoundTrip_Structure_GenotypePayload) {
//  // Setup test data
//  uint8_t max_ploidy = 3;
//  bool no_reference_flag = true;
//  bool not_available_flag = false;
//  bool phases_value = true;
//
//  // Create test payloads
//  std::vector<SortedBinMatPayload> variants_payloads;
//  variants_payloads.emplace_back(1, 2, 3, genie::core::AlgoID::JBIG);
//  variants_payloads.emplace_back(4, 5, 6, genie::core::AlgoID::ZSTD);
//
//  std::optional<AmaxPayload> variants_amax_payload = AmaxPayload({10, 20, 30});
//  std::optional<SortedBinMatPayload> phases_payload = SortedBinMatPayload(2, 3, 4, genie::core::AlgoID::BSC);
//
//  // Create GenotypeParameters
//  genie::genotype::GenotypeParameters params;
//  params.SetBinarizationID(genie::genotype::BinarizationID::ROW_BIN);
//  params.SetEncodePhasesDataFlag(true);
//  params.SetAllelesCodecID(genie::core::AlgoID::JBIG);
//  params.SetPhasesCodecID(genie::core::AlgoID::BSC);
//  params.SetSortAllelesRowsFlag(true);
//  params.SetSortAllelesColsFlag(true);
//  params.SetSortPhasesRowsFlag(true);
//  params.SetSortPhasesColsFlag(true);
//
//  // Create entropy encoders
//  genie::entropy::Encoder* alleles_encoder = new genie::entropy::BSCEncoder();
//  genie::entropy::Encoder* phases_encoder = new genie::entropy::JBigEncoder();
//
//  // Create initial GenotypePayload
//  genie::genotype::GenotypePayload orig_payload(
//      max_ploidy,
//      no_reference_flag,
//      not_available_flag,
//      phases_value,
//      std::move(variants_payloads),
//      std::move(variants_amax_payload),
//      std::move(phases_payload)
//  );
//
//  // Serialize to bitstream
//  std::stringstream bitstream;
//  genie::util::BitWriter writer(&bitstream);
//  orig_payload.Write(writer);
//
//  // Deserialize from bitstream
//  std::istream& reader = bitstream;
//  genie::util::BitReader bit_reader(reader);
//  genie::genotype::GenotypePayload recon_payload(bit_reader, params);
//
//  // Verify reconstructed payload matches original
//  EXPECT_EQ(orig_payload.GetMaxPloidy(), recon_payload.GetMaxPloidy());
//  EXPECT_EQ(orig_payload.GetNoReferenceFlag(), recon_payload.GetNoReferenceFlag());
//  EXPECT_EQ(orig_payload.GetNotAvailableFlag(), recon_payload.GetNotAvailableFlag());
//  EXPECT_EQ(orig_payload.GetPhasesValue(), recon_payload.GetPhasesValue());
//
//  // Verify variants payloads
//  const auto& orig_variants = orig_payload.GetVariantsPayloads();
//  const auto& recon_variants = recon_payload.GetVariantsPayloads();
//  EXPECT_EQ(orig_variants.size(), recon_variants.size());
//  for (size_t i = 0; i < orig_variants.size(); ++i) {
//    EXPECT_EQ(orig_variants[i], recon_variants[i]);
//  }
//
//  // Verify amax payload
//  EXPECT_EQ(orig_payload.IsAmaxPayloadExist(), recon_payload.IsAmaxPayloadExist());
//  if (orig_payload.IsAmaxPayloadExist()) {
//    EXPECT_EQ(orig_payload.GetVariantsAmaxPayload().value(),
//              recon_payload.GetVariantsAmaxPayload().value());
//  }
//
//  // Verify phases payload
//  EXPECT_EQ(orig_payload.EncodePhaseValues(), recon_payload.EncodePhaseValues());
//  if (orig_payload.EncodePhaseValues()) {
//    EXPECT_EQ(orig_payload.GetPhasesPayload().value(),
//              recon_payload.GetPhasesPayload().value());
//  }
}

// -----------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------------------------------------------------

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


// ---------------------------------------------------------------------------------------------------------------------
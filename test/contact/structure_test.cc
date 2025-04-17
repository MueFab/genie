/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <cmath>  // For fabs function
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/contact/contact_coder.h"
#include "genie/contact/contact_matrix_bin_payload.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactStructure, RoundTrip_Structure_ContactMatrixTilePayload){

    std::srand((unsigned)std::time(0)); // seed the random number generator

    // Test JBIG
    {
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        auto NROWS = 100u;
        auto NCOLS = 200u;
        auto PAYLOAD_SIZE = 128u;
        auto PAYLOAD = std::vector<uint8_t>(PAYLOAD_SIZE);
        for (auto &v: PAYLOAD){
            v = static_cast<uint8_t>(std::rand() % 256);
        }

        auto payload = std::vector<uint8_t>(PAYLOAD);

        auto orig_obj = genie::contact::ContactMatrixTilePayload(
            CODEC_ID,
            NROWS,
            NCOLS,
            std::move(payload)
        );

        ASSERT_EQ(orig_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(orig_obj.GetTileNRows(), 0u);
        ASSERT_EQ(orig_obj.GetTileNCols(), 0u);
        ASSERT_EQ(orig_obj.GetPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(orig_obj.GetPayload(), PAYLOAD);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        auto tile_payload_size = obj_payload.str().size();
        ASSERT_EQ(tile_payload_size, orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixTilePayload(
            bitreader,
            tile_payload_size
        );

        ASSERT_EQ(recon_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.GetTileNRows(), 0u);
        ASSERT_EQ(recon_obj.GetTileNCols(), 0u);
        ASSERT_EQ(recon_obj.GetPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(recon_obj.GetPayload(), PAYLOAD);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // Test other codec
    {
        auto CODEC_ID = genie::core::AlgoID::BSC;
        auto NROWS = 100u;
        auto NCOLS = 200u;
        auto PAYLOAD_SIZE = 128u;
        auto PAYLOAD = std::vector<uint8_t>(PAYLOAD_SIZE);
        for (auto &v: PAYLOAD){
            v = static_cast<uint8_t>(std::rand() % 256);
        }

        auto payload = std::vector<uint8_t>(PAYLOAD);

        auto orig_obj = genie::contact::ContactMatrixTilePayload(
            CODEC_ID,
            NROWS,
            NCOLS,
            std::move(payload)
        );

        ASSERT_EQ(orig_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(orig_obj.GetTileNRows(), NROWS);
        ASSERT_EQ(orig_obj.GetTileNCols(), NCOLS);
        ASSERT_EQ(orig_obj.GetPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(orig_obj.GetPayload(), PAYLOAD);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        auto tile_payload_size = obj_payload.str().size();
        ASSERT_EQ(tile_payload_size, orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixTilePayload(
            bitreader,
            tile_payload_size
        );

        ASSERT_EQ(recon_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.GetTileNRows(), NROWS);
        ASSERT_EQ(recon_obj.GetTileNCols(), NCOLS);
        ASSERT_EQ(recon_obj.GetPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(recon_obj.GetPayload(), PAYLOAD);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactStructure, RoundTrip_Structure_SubcontactMatrixMaskPayload){

    // TransformID 0
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_0;
        auto NUM_BIN_ENTRIES = 100u;

        genie::contact::BinVecDtype MASK_ARRAY = xt::cast<bool>(xt::random::randint<uint16_t>({NUM_BIN_ENTRIES}, 0, 2));

        // Default Constructor
        {
            genie::contact::BinVecDtype orig_mask_arr = genie::contact::BinVecDtype(MASK_ARRAY);

            auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
            orig_obj.SetTransformId(TRANSFORM_ID);
            orig_obj.SetMaskArray(orig_mask_arr);

            auto obj_payload = std::stringstream();
            std::ostream& writer = obj_payload;
            auto bitwriter = genie::util::BitWriter(&writer);
            orig_obj.Write(bitwriter);

            ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

            std::istream& reader = obj_payload;
            auto bitreader = genie::util::BitReader(reader);
            auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
                bitreader,
                NUM_BIN_ENTRIES
            );

            ASSERT_EQ(recon_obj.GetTransformID(), TRANSFORM_ID);
            for (auto i = 0u; i < NUM_BIN_ENTRIES; i++){
                ASSERT_EQ(recon_obj.GetMaskArray()[i], MASK_ARRAY(i));
            }
            ASSERT_EQ(recon_obj.GetFirstVal(), MASK_ARRAY(0));
            ASSERT_EQ(recon_obj.AnyRlEntries(), false);

            ASSERT_TRUE(orig_obj == recon_obj);
        }

        // Move Constructor
        {
            genie::contact::BinVecDtype orig_mask_arr = genie::contact::BinVecDtype(MASK_ARRAY);

            auto orig_obj = genie::contact::SubcontactMatrixMaskPayload(
                std::move(orig_mask_arr)
            );

            auto obj_payload = std::stringstream();
            std::ostream& writer = obj_payload;
            auto bitwriter = genie::util::BitWriter(&writer);
            orig_obj.Write(bitwriter);

            ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

            std::istream& reader = obj_payload;
            auto bitreader = genie::util::BitReader(reader);
            auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
                bitreader,
                NUM_BIN_ENTRIES
            );

            ASSERT_EQ(recon_obj.GetTransformID(), TRANSFORM_ID);
            for (auto i = 0u; i < NUM_BIN_ENTRIES; i++){
                ASSERT_EQ(recon_obj.GetMaskArray()[i], MASK_ARRAY(i));
            }
            ASSERT_EQ(recon_obj.GetFirstVal(), MASK_ARRAY(0));
            ASSERT_EQ(recon_obj.AnyRlEntries(), false);

            ASSERT_TRUE(orig_obj == recon_obj);
        }
    }

    // TransformID 1
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_1;
        auto NUM_RL_ENTRIES = 10u;
        auto MIN_VAL = 1;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint8_t>(
            static_cast<uint8_t>(MIN_VAL),
            static_cast<uint8_t>(MAX_VAL),
            NUM_RL_ENTRIES
        );
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.SetRlEntries(TRANSFORM_ID, FIRST_VAL, orig_rl_entries);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.GetTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.AnyMaskArray());
        ASSERT_EQ(recon_obj.GetFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.GetRlEntries()[i], RL_ENTRIES(i));
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // TransformID 2
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_2;
        auto NUM_RL_ENTRIES = 10u;
        auto MIN_VAL = 1;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint16_t>(
            static_cast<uint16_t>(MIN_VAL),
            static_cast<uint16_t>(MAX_VAL),
            NUM_RL_ENTRIES
        );
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.SetRlEntries(TRANSFORM_ID, FIRST_VAL, orig_rl_entries);
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.GetTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.AnyMaskArray());
        ASSERT_EQ(recon_obj.GetFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.GetRlEntries()[i], RL_ENTRIES(i));
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // TransformID 3
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_3;
        auto NUM_RL_ENTRIES = 10u;
        auto MIN_VAL = 1;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint32_t>(
            static_cast<uint32_t>(MIN_VAL),
            static_cast<uint32_t>(MAX_VAL),
            NUM_RL_ENTRIES
        );
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.SetRlEntries(TRANSFORM_ID, FIRST_VAL, orig_rl_entries);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.GetTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.AnyMaskArray());
        ASSERT_EQ(recon_obj.GetFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.GetRlEntries()[i], RL_ENTRIES(i));
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactStructure, RoundTrip_Structure_ContactMatrixParameter){
    {
        auto MULTS = std::vector<uint32_t>({1, 2, 4, 5});
        uint16_t SAMPLE1_ID = 10u;
        auto SAMPLE1_NAME = std::string("SAMPLE1");
        uint16_t SAMPLE2_ID = 20u;
        auto SAMPLE2_NAME = std::string("SAMPLE2");

        constexpr auto CHR1_ID = 30u;
        constexpr auto CHR1_NAME = "CHR1";
        constexpr auto CHR1_LEN = 70u;
        constexpr auto CHR2_ID = 40u;
        constexpr auto CHR2_NAME = "CHR2";
        constexpr auto CHR2_LEN = CHR1_LEN;

        auto BIN_SIZE = 5u;
        auto TILE_SIZE = 5u;

        // uint32_t MULTIPLIER = 1u;

        auto ORIG_CM_PARAM = genie::contact::ContactMatrixParameters();
        ORIG_CM_PARAM.UpsertChromosome(CHR1_ID, CHR1_NAME, CHR1_LEN);
        ORIG_CM_PARAM.UpsertChromosome(CHR2_ID, CHR2_NAME, CHR2_LEN);
        ORIG_CM_PARAM.SetBinSize(BIN_SIZE);
        ORIG_CM_PARAM.SetTileSize(TILE_SIZE);
        ORIG_CM_PARAM.AddSample(SAMPLE1_ID, std::string(SAMPLE1_NAME), true);
        ORIG_CM_PARAM.AddSample(SAMPLE2_ID, std::string(SAMPLE2_NAME), true);

        ASSERT_EQ(ORIG_CM_PARAM.GetBinSize(), BIN_SIZE);
        ASSERT_EQ(ORIG_CM_PARAM.GetTileSize(), TILE_SIZE);
        ASSERT_EQ(ORIG_CM_PARAM.GetNumSamples(), 2u);
        ASSERT_EQ(ORIG_CM_PARAM.GetChromosomeLength(CHR1_ID), CHR1_LEN);
        ASSERT_EQ(ORIG_CM_PARAM.GetChromosomeLength(CHR2_ID), CHR2_LEN);
        ASSERT_EQ(ORIG_CM_PARAM.GetSampleName(SAMPLE1_ID), SAMPLE1_NAME);
        ASSERT_EQ(ORIG_CM_PARAM.GetSampleName(SAMPLE2_ID), SAMPLE2_NAME);

        std::stringstream obj_payload;
        std::ostream& writer = obj_payload;
        auto CMWriter = genie::core::Writer(&writer);
        ORIG_CM_PARAM.Write(CMWriter);

        ASSERT_EQ(obj_payload.str().size(), ORIG_CM_PARAM.GetSize());

        std::istream& reader = obj_payload;
        auto bitReader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixParameters(bitReader);

        ASSERT_EQ(recon_obj.GetBinSize(), BIN_SIZE);
        ASSERT_EQ(recon_obj.GetTileSize(), TILE_SIZE);
        ASSERT_EQ(recon_obj.GetNumSamples(), 2u);
        ASSERT_EQ(recon_obj.GetChromosomeLength(CHR1_ID), CHR1_LEN);
        ASSERT_EQ(recon_obj.GetChromosomeLength(CHR2_ID), CHR2_LEN);
        ASSERT_EQ(recon_obj.GetSampleName(SAMPLE1_ID), SAMPLE1_NAME);
        ASSERT_EQ(recon_obj.GetSampleName(SAMPLE2_ID), SAMPLE2_NAME);

        ASSERT_TRUE(ORIG_CM_PARAM == recon_obj);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactStructure, RoundTrip_Structure_SubcontactMatrixParameter){
    // Intra-SCM case
    {
        auto BINARIZATION_MODE = genie::contact::BinarizationMode::ROW_BINARIZATION;
        auto DIAG_MODE = genie::contact::DiagonalTransformMode::MODE_0;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        uint8_t CHR1_ID = 0u;
        auto CHR1_NAME = std::string("CHR1");
        auto CHR1_LEN = 70u;
        uint8_t CHR2_ID = CHR1_ID;
        auto CHR2_NAME = std::string(CHR1_NAME);
        auto CHR2_LEN = CHR1_LEN;
        uint32_t BIN_SIZE = 5u;
        uint32_t TILE_SIZE = 5u;
        uint32_t MULTIPLIER = 1u;

        auto ORIG_CM_PARAM = genie::contact::ContactMatrixParameters();
        ORIG_CM_PARAM.UpsertChromosome(CHR1_ID, CHR1_NAME, CHR1_LEN);
        ORIG_CM_PARAM.UpsertChromosome(CHR2_ID, CHR2_NAME, CHR2_LEN);
        ORIG_CM_PARAM.SetBinSize(BIN_SIZE);
        ORIG_CM_PARAM.SetTileSize(TILE_SIZE);
        auto NTILES_IN_ROW = ORIG_CM_PARAM.GetNumTiles(CHR1_ID, MULTIPLIER);
        auto NTILES_IN_COL = ORIG_CM_PARAM.GetNumTiles(CHR2_ID, MULTIPLIER);

        auto ORIG_SCM_PARAM = genie::contact::SubcontactMatrixParameters();
        ORIG_SCM_PARAM.SetCodecID(CODEC_ID);
        ORIG_SCM_PARAM.SetChr1ID(CHR1_ID);
        ORIG_SCM_PARAM.SetChr2ID(CHR2_ID);
        ORIG_SCM_PARAM.SetNumTiles(NTILES_IN_ROW, NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){

                if (!(i>j && ORIG_SCM_PARAM.IsIntraSCM())){
                    auto tile_param = genie::contact::TileParameter();
                    tile_param.binarization_mode = BINARIZATION_MODE;
                    tile_param.diag_tranform_mode = DIAG_MODE;

                    ORIG_SCM_PARAM.SetTileParameter(i, j, tile_param);
                }
            }
        }

        auto orig_obj = genie::contact::SubcontactMatrixParameters(ORIG_SCM_PARAM);
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixParameters(
            bitreader,
            ORIG_CM_PARAM
        );

        ASSERT_EQ(recon_obj.GetChr1ID(), CHR1_ID);
        ASSERT_EQ(recon_obj.GetChr2ID(), CHR2_ID);
        ASSERT_EQ(recon_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.GetNTilesInRow(), NTILES_IN_ROW);
        ASSERT_EQ(recon_obj.GetNTilesInCol(), NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){
                if (!(i>j && orig_obj.IsIntraSCM())){
                    auto& orig_tile_param = orig_obj.GetTileParameter(i, j);
                    auto& recon_tile_param = orig_obj.GetTileParameter(i, j);

                    ASSERT_EQ(
                        orig_tile_param.diag_tranform_mode,
                        recon_tile_param.diag_tranform_mode
                    );

                    ASSERT_EQ(
                        orig_tile_param.binarization_mode,
                        recon_tile_param.binarization_mode
                    );
                }
            }
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // Intra-SCM case
    {
        auto BINARIZATION_MODE = genie::contact::BinarizationMode::ROW_BINARIZATION;
        auto DIAG_MODE = genie::contact::DiagonalTransformMode::MODE_0;
        auto CODEC_ID = genie::core::AlgoID::CABAC;
        uint8_t CHR1_ID = 0u;
        auto CHR1_NAME = std::string("CHR1");
        auto CHR1_LEN = 70u;
        uint8_t CHR2_ID = 1u;
        auto CHR2_NAME = std::string("CHR2");
        auto CHR2_LEN = 48u;
        uint32_t BIN_SIZE = 5u;
        uint32_t TILE_SIZE = 5u;
        uint32_t MULTIPLIER = 1u;

        auto ORIG_CM_PARAM = genie::contact::ContactMatrixParameters();
        ORIG_CM_PARAM.SetBinSize(BIN_SIZE);
        ORIG_CM_PARAM.SetTileSize(TILE_SIZE);
        ORIG_CM_PARAM.UpsertChromosome(static_cast<uint8_t>(CHR1_ID), CHR1_NAME,
                                       CHR1_LEN);
        ORIG_CM_PARAM.UpsertChromosome(static_cast<uint8_t>(CHR2_ID), CHR2_NAME,
                                       CHR2_LEN);
        auto NTILES_IN_ROW = ORIG_CM_PARAM.GetNumTiles(CHR1_ID, MULTIPLIER);
        auto NTILES_IN_COL = ORIG_CM_PARAM.GetNumTiles(CHR2_ID, MULTIPLIER);

        auto ORIG_SCM_PARAM = genie::contact::SubcontactMatrixParameters();
        ORIG_SCM_PARAM.SetCodecID(CODEC_ID);
        ORIG_SCM_PARAM.SetChr1ID(CHR1_ID);
        ORIG_SCM_PARAM.SetChr2ID(CHR2_ID);
        ORIG_SCM_PARAM.SetNumTiles(NTILES_IN_ROW, NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){

                if (!(i>j && ORIG_SCM_PARAM.IsIntraSCM())){
                    auto tile_param = genie::contact::TileParameter();
                    tile_param.binarization_mode = BINARIZATION_MODE;
                    tile_param.diag_tranform_mode = DIAG_MODE;

                    ORIG_SCM_PARAM.SetTileParameter(i, j, tile_param);
                }
            }
        }

        auto orig_obj = genie::contact::SubcontactMatrixParameters(
            ORIG_SCM_PARAM
        );
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.Write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.GetSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixParameters(
            bitreader,
            ORIG_CM_PARAM
        );

        ASSERT_EQ(recon_obj.GetChr1ID(), CHR1_ID);
        ASSERT_EQ(recon_obj.GetChr2ID(), CHR2_ID);
        ASSERT_EQ(recon_obj.GetCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.GetNTilesInRow(), NTILES_IN_ROW);
        ASSERT_EQ(recon_obj.GetNTilesInCol(), NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){
                if (!(i>j && orig_obj.IsIntraSCM())){
                    auto& orig_tile_param = orig_obj.GetTileParameter(i, j);
                    auto& recon_tile_param = orig_obj.GetTileParameter(i, j);

                    ASSERT_EQ(
                        orig_tile_param.diag_tranform_mode,
                        recon_tile_param.diag_tranform_mode
                    );

                    ASSERT_EQ(
                        orig_tile_param.binarization_mode,
                        recon_tile_param.binarization_mode
                    );
                }
            }
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactStructure, RoundTrip_Structure_ContactMatrixBinPayload){
  const double TOLERANCE = 1e-6;

  std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
  std::string filename = "GSE63525_GM12878_insitu_primary_30.hic-raw-";
  auto RESOLUTION = 250000u;
  std::vector<std::string> NORM_NAMES = {
    std::string("GW_KR"),
    std::string("GW_VC"),
    std::string("INTER_KR"),
    std::string("INTER_VC"),
    std::string("KR"),
    std::string("VC"),
    std::string("VC_SQRT")
  };

  uint16_t SAMPLE_ID = 0;
  uint32_t BIN_SIZE_MULT = 5;
  size_t NUM_NORM_METHODS = 7;

  std::vector<uint8_t> CHROMS = {
    21, // 193
    22  // 206
  };

  std::vector<uint8_t> LENGTHS = {
    193, // 193
    206  // 206
  };

  std::vector<uint8_t> POSS = {
    192, // 193
    204  // 206
  };


  std::vector<std::vector<double>> ORIV_VALS = {
    // GW_KR, GW_VC, INTER_KR, INTER_VC, KR, VC, VC_SQRT
    {0.561230,  0.579038,  0.781984,  0.952380,  0.564981,  1.311789,  0.679117}, // Chrom 21 idx 192
    {1.015586,  1.256156,  1.517353,  1.897584,  0.788887,  1.165799,  0.839396} // Chrom 22 idx 204
  };

  auto chrom_idx = 0u;
  for (auto& chrom:  CHROMS){

    auto norm_idx = 0u;

    genie::contact::ContactMatrixBinPayload bin_payload;
    bin_payload.SetSampleID(SAMPLE_ID);
    bin_payload.SetChrID(CHROMS[chrom_idx]);
    bin_payload.SetBinSizeMultiplier(BIN_SIZE_MULT);

    for (auto& norm_name: NORM_NAMES){
      std::string filepath = gitRootDir + "/data/records/contact/" + filename + std::to_string(RESOLUTION) + "." + std::to_string(chrom) + "." + norm_name + ".weights";

      bin_payload.ReadWeightValuesFromFile(filepath);

      ASSERT_EQ(bin_payload.GetWeightValue()[norm_idx].size(), LENGTHS[chrom_idx]);
      ASSERT_TRUE(fabs((bin_payload.GetWeightValue()[norm_idx][POSS[chrom_idx]] - ORIV_VALS[chrom_idx][norm_idx]) - TOLERANCE));

      norm_idx++;
    }

    auto obj_payload = std::stringstream();
    std::ostream& writer = obj_payload;
    auto bit_writer = genie::util::BitWriter(&writer);
    bin_payload.Write(bit_writer);

    std::istream& reader = obj_payload;
    auto bit_reader = genie::util::BitReader(reader);
    auto recon_obj = genie::contact::ContactMatrixBinPayload(
        bit_reader,
        static_cast<uint8_t>(NUM_NORM_METHODS),
        static_cast<uint32_t>(LENGTHS[chrom_idx])
    );

    ASSERT_EQ(bin_payload.GetSampleID(), recon_obj.GetSampleID());
    ASSERT_EQ(bin_payload.GetChrID(), recon_obj.GetChrID());
    ASSERT_EQ(bin_payload.GetBinSizeMultiplier(), recon_obj.GetBinSizeMultiplier());

    ASSERT_EQ(bin_payload.GetNumNormMethods(), recon_obj.GetNumNormMethods());
    auto orig_weight_values = bin_payload.GetWeightValue();
    auto recon_weight_values = recon_obj.GetWeightValue();
    for (auto i = 0u; i < bin_payload.GetNumNormMethods(); i++){
      auto orig_norm_weight_values = orig_weight_values[i];
      auto recon_norm_weight_values = recon_weight_values[i];

      ASSERT_EQ(orig_norm_weight_values.size(), recon_norm_weight_values.size());
      auto num_entries = orig_norm_weight_values.size();

      for (auto j = 0u; j<num_entries; j++){
        auto orig_weight_value = orig_norm_weight_values[j];
        auto recon_weight_value = recon_norm_weight_values[j];
        auto orig_weight_value_int = *reinterpret_cast<uint64_t*>(&orig_weight_value);
        auto recon_weight_value_int = *reinterpret_cast<uint64_t*>(&recon_weight_value);

        ASSERT_EQ(orig_weight_value_int, recon_weight_value_int) << "i:" << i << "j:" << j;
      }
    }
    ASSERT_TRUE(bin_payload == recon_obj); // TODO(Yeremia): Fix the comparison

    chrom_idx++;
  }

}

// ---------------------------------------------------------------------------------------------------------------------
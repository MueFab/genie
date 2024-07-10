/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/core/constants.h"
#include "genie/contact/contact_coder.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Structure_ContactMatrixTilePayload){

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

        ASSERT_EQ(orig_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(orig_obj.getTileNRows(), 0u);
        ASSERT_EQ(orig_obj.getTileNCols(), 0u);
        ASSERT_EQ(orig_obj.getPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(orig_obj.getPayload(), PAYLOAD);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixTilePayload(
            bitreader
        );

        ASSERT_EQ(recon_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.getTileNRows(), 0u);
        ASSERT_EQ(recon_obj.getTileNCols(), 0u);
        ASSERT_EQ(recon_obj.getPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(recon_obj.getPayload(), PAYLOAD);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // Test other codec
    {
        auto CODEC_ID = genie::core::AlgoID::CABAC;
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

        ASSERT_EQ(orig_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(orig_obj.getTileNRows(), NROWS);
        ASSERT_EQ(orig_obj.getTileNCols(), NCOLS);
        ASSERT_EQ(orig_obj.getPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(orig_obj.getPayload(), PAYLOAD);

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixTilePayload(
            bitreader
        );

        ASSERT_EQ(recon_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.getTileNRows(), NROWS);
        ASSERT_EQ(recon_obj.getTileNCols(), NCOLS);
        ASSERT_EQ(recon_obj.getPayloadSize(), PAYLOAD_SIZE);
        ASSERT_EQ(recon_obj.getPayload(), PAYLOAD);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Structure_SubcontactMatrixMaskPayload){

    // TransformID 0
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_0;
        auto NUM_BIN_ENTRIES = 100u;

        genie::contact::BinVecDtype MASK_ARRAY = xt::cast<bool>(xt::random::randint<uint16_t>({NUM_BIN_ENTRIES}, 0, 2));

        // Default Constructor
        {
            genie::contact::BinVecDtype orig_mask_arr = genie::contact::BinVecDtype(MASK_ARRAY);

            auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
            orig_obj.setTransformID(TRANSFORM_ID);
            orig_obj.setMaskArray(orig_mask_arr);

            auto obj_payload = std::stringstream();
            std::ostream& writer = obj_payload;
            auto bitwriter = genie::util::BitWriter(&writer);
            orig_obj.write(bitwriter);

            ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

            std::istream& reader = obj_payload;
            auto bitreader = genie::util::BitReader(reader);
            auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
                bitreader,
                NUM_BIN_ENTRIES
            );

            ASSERT_EQ(recon_obj.getTransformID(), TRANSFORM_ID);
            for (auto i = 0u; i < NUM_BIN_ENTRIES; i++){
                ASSERT_EQ(recon_obj.getMaskArray()[i], MASK_ARRAY(i));
            }
            ASSERT_EQ(recon_obj.getFirstVal(), MASK_ARRAY(0));
            ASSERT_EQ(recon_obj.anyRLEntries(), false);

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
            orig_obj.write(bitwriter);

            ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

            std::istream& reader = obj_payload;
            auto bitreader = genie::util::BitReader(reader);
            auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
                bitreader,
                NUM_BIN_ENTRIES
            );

            ASSERT_EQ(recon_obj.getTransformID(), TRANSFORM_ID);
            for (auto i = 0u; i < NUM_BIN_ENTRIES; i++){
                ASSERT_EQ(recon_obj.getMaskArray()[i], MASK_ARRAY(i));
            }
            ASSERT_EQ(recon_obj.getFirstVal(), MASK_ARRAY(0));
            ASSERT_EQ(recon_obj.anyRLEntries(), false);

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
        orig_obj.setRLEntries(
            TRANSFORM_ID,
            FIRST_VAL,
            orig_rl_entries
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.getTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.anyMaskArray());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.getRLEntries()[i], RL_ENTRIES(i));
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
        orig_obj.setRLEntries(
            TRANSFORM_ID,
            FIRST_VAL,
            orig_rl_entries
        );
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.getTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.anyMaskArray());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.getRLEntries()[i], RL_ENTRIES(i));
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
        orig_obj.setRLEntries(
            TRANSFORM_ID,
            FIRST_VAL,
            orig_rl_entries
        );

        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixMaskPayload(
            bitreader,
            NUM_BIN_ENTRIES
        );

        ASSERT_EQ(recon_obj.getTransformID(), TRANSFORM_ID);
        ASSERT_FALSE(recon_obj.anyMaskArray());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        for (auto i = 0u; i < NUM_RL_ENTRIES; i++){
            ASSERT_EQ(recon_obj.getRLEntries()[i], RL_ENTRIES(i));
        }

        ASSERT_TRUE(orig_obj == recon_obj);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Structure_ContactMatrixParameter){
    {
        auto MULTS = std::vector<uint32_t>({1, 2, 4, 5});
        uint8_t SAMPLE1_ID = 10u;
        auto SAMPLE1_NAME = std::string("SAMPLE1");
        uint8_t SAMPLE2_ID = 20u;
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
        ORIG_CM_PARAM.upsertChromosome(CHR1_ID, CHR1_NAME, CHR1_LEN);
        ORIG_CM_PARAM.upsertChromosome(CHR2_ID, CHR2_NAME, CHR2_LEN);
        ORIG_CM_PARAM.setBinSize(BIN_SIZE);
        ORIG_CM_PARAM.setTileSize(TILE_SIZE);
        ORIG_CM_PARAM.addSample(SAMPLE1_ID, std::string(SAMPLE1_NAME), true);
        ORIG_CM_PARAM.addSample(SAMPLE2_ID, std::string(SAMPLE2_NAME), true);

        ASSERT_EQ(ORIG_CM_PARAM.getBinSize(), BIN_SIZE);
        ASSERT_EQ(ORIG_CM_PARAM.getTileSize(), TILE_SIZE);
        ASSERT_EQ(ORIG_CM_PARAM.getNumSamples(), 2u);
        ASSERT_EQ(ORIG_CM_PARAM.getChromosomeLength(CHR1_ID), CHR1_LEN);
        ASSERT_EQ(ORIG_CM_PARAM.getChromosomeLength(CHR2_ID), CHR2_LEN);
        ASSERT_EQ(ORIG_CM_PARAM.getSampleName(SAMPLE1_ID), SAMPLE1_NAME);
        ASSERT_EQ(ORIG_CM_PARAM.getSampleName(SAMPLE2_ID), SAMPLE2_NAME);

        std::stringstream obj_payload;
        std::ostream& writer = obj_payload;
        auto CMWriter = genie::core::Writer(&writer);
        ORIG_CM_PARAM.write(CMWriter);

        ASSERT_EQ(obj_payload.str().size(), ORIG_CM_PARAM.getSize());

        std::istream& reader = obj_payload;
        auto bitReader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::ContactMatrixParameters(bitReader);

        ASSERT_EQ(recon_obj.getBinSize(), BIN_SIZE);
        ASSERT_EQ(recon_obj.getTileSize(), TILE_SIZE);
        ASSERT_EQ(recon_obj.getNumSamples(), 2u);
        ASSERT_EQ(recon_obj.getChromosomeLength(CHR1_ID), CHR1_LEN);
        ASSERT_EQ(recon_obj.getChromosomeLength(CHR2_ID), CHR2_LEN);
        ASSERT_EQ(recon_obj.getSampleName(SAMPLE1_ID), SAMPLE1_NAME);
        ASSERT_EQ(recon_obj.getSampleName(SAMPLE2_ID), SAMPLE2_NAME);

        ASSERT_TRUE(ORIG_CM_PARAM==recon_obj);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_Structure_SubcontactMatrixParameter){
    // Intra-SCM case
    {
        auto BINARIZATION_MODE = genie::contact::BinarizationMode::ROW_BINARIZATION;
        auto DIAG_MODE = genie::contact::DiagonalTransformMode::MODE_0;
        auto CODEC_ID = genie::core::AlgoID::JBIG;
        uint8_t CHR1_ID = 0u;
        auto CHR1_NAME = std::string("CHR1");
        auto CHR1_LEN = 70u;
        uint8_t CHR2_ID = 0u;
        auto CHR2_NAME = std::string(CHR1_NAME);
        auto CHR2_LEN = CHR1_LEN;
        uint32_t BIN_SIZE = 5u;
        uint32_t TILE_SIZE = 5u;
        uint32_t MULTIPLIER = 1u;

        auto ORIG_CM_PARAM = genie::contact::ContactMatrixParameters();
        ORIG_CM_PARAM.upsertChromosome(CHR1_ID, CHR1_NAME, CHR1_LEN);
        ORIG_CM_PARAM.upsertChromosome(CHR2_ID, CHR2_NAME, CHR2_LEN);
        ORIG_CM_PARAM.setBinSize(BIN_SIZE);
        ORIG_CM_PARAM.setTileSize(TILE_SIZE);
        auto NTILES_IN_ROW = ORIG_CM_PARAM.getNumTiles(CHR1_ID, MULTIPLIER);
        auto NTILES_IN_COL = ORIG_CM_PARAM.getNumTiles(CHR2_ID, MULTIPLIER);

        auto ORIG_SCM_PARAM = genie::contact::SubcontactMatrixParameters();
        ORIG_SCM_PARAM.setCodecID(CODEC_ID);
        ORIG_SCM_PARAM.setChr1ID(CHR1_ID);
        ORIG_SCM_PARAM.setChr2ID(CHR2_ID);
        ORIG_SCM_PARAM.setNumTiles(NTILES_IN_ROW, NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){

                if (!(i>j && ORIG_SCM_PARAM.isIntraSCM())){
                    auto tile_param = genie::contact::TileParameter();
                    tile_param.binarization_mode = BINARIZATION_MODE;
                    tile_param.diag_tranform_mode = DIAG_MODE;

                    ORIG_SCM_PARAM.setTileParameter(
                        i,
                        j,
                        tile_param
                    );
                }
            }
        }

        auto orig_obj = genie::contact::SubcontactMatrixParameters(ORIG_SCM_PARAM);
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixParameters(
            bitreader,
            ORIG_CM_PARAM
        );

        ASSERT_EQ(recon_obj.getChr1ID(), CHR1_ID);
        ASSERT_EQ(recon_obj.getChr2ID(), CHR2_ID);
        ASSERT_EQ(recon_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.getNTilesInRow(), NTILES_IN_ROW);
        ASSERT_EQ(recon_obj.getNTilesInCol(), NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){
                if (!(i>j && orig_obj.isIntraSCM())){
                    auto& orig_tile_param = orig_obj.getTileParameter(i, j);
                    auto& recon_tile_param = orig_obj.getTileParameter(i, j);

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
        ORIG_CM_PARAM.setBinSize(BIN_SIZE);
        ORIG_CM_PARAM.setTileSize(TILE_SIZE);
        ORIG_CM_PARAM.upsertChromosome(
            static_cast<uint8_t>(CHR1_ID),
            CHR1_NAME,
            CHR1_LEN
        );
        ORIG_CM_PARAM.upsertChromosome(
            static_cast<uint8_t>(CHR2_ID),
            CHR2_NAME,
            CHR2_LEN
        );
        auto NTILES_IN_ROW = ORIG_CM_PARAM.getNumTiles(CHR1_ID, MULTIPLIER);
        auto NTILES_IN_COL = ORIG_CM_PARAM.getNumTiles(CHR2_ID, MULTIPLIER);

        auto ORIG_SCM_PARAM = genie::contact::SubcontactMatrixParameters();
        ORIG_SCM_PARAM.setCodecID(CODEC_ID);
        ORIG_SCM_PARAM.setChr1ID(CHR1_ID);
        ORIG_SCM_PARAM.setChr2ID(CHR2_ID);
        ORIG_SCM_PARAM.setNumTiles(NTILES_IN_ROW, NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){

                if (!(i>j && ORIG_SCM_PARAM.isIntraSCM())){
                    auto tile_param = genie::contact::TileParameter();
                    tile_param.binarization_mode = BINARIZATION_MODE;
                    tile_param.diag_tranform_mode = DIAG_MODE;

                    ORIG_SCM_PARAM.setTileParameter(
                        i,
                        j,
                        tile_param
                    );
                }
            }
        }

        auto orig_obj = genie::contact::SubcontactMatrixParameters(
            ORIG_SCM_PARAM
        );
        auto obj_payload = std::stringstream();
        std::ostream& writer = obj_payload;
        auto bitwriter = genie::util::BitWriter(&writer);
        orig_obj.write(bitwriter);

        ASSERT_EQ(obj_payload.str().size(), orig_obj.getSize());

        std::istream& reader = obj_payload;
        auto bitreader = genie::util::BitReader(reader);
        auto recon_obj = genie::contact::SubcontactMatrixParameters(
            bitreader,
            ORIG_CM_PARAM
        );

        ASSERT_EQ(recon_obj.getChr1ID(), CHR1_ID);
        ASSERT_EQ(recon_obj.getChr2ID(), CHR2_ID);
        ASSERT_EQ(recon_obj.getCodecID(), CODEC_ID);
        ASSERT_EQ(recon_obj.getNTilesInRow(), NTILES_IN_ROW);
        ASSERT_EQ(recon_obj.getNTilesInCol(), NTILES_IN_COL);

        for (size_t i = 0u; i<NTILES_IN_ROW; i++){
            for (size_t j = 0u; j<NTILES_IN_COL; j++){
                if (!(i>j && orig_obj.isIntraSCM())){
                    auto& orig_tile_param = orig_obj.getTileParameter(i, j);
                    auto& recon_tile_param = orig_obj.getTileParameter(i, j);

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

TEST(ContactCoder, RoundTrip_Structure_SubcontactMatrixPayload){
    // Intra SCM
    {

    }

    // Inter SCM
    {

    }
}

// ---------------------------------------------------------------------------------------------------------------------
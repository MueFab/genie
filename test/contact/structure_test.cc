/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "genie/contact/contact_coder.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
//#include "genie/util/runtime-exception.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(ContactCoder, RoundTrip_ContactMatrixTilePayload){

    std::srand(std::time(0)); // seed the random number generator

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

TEST(ContactCoder, RoundTrip_SubcontactMatrixMaskPayload){

    // TransformID 0
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_0;
        auto NUM_BIN_ENTRIES = 100u;
        genie::contact::BinVecDtype MASK_ARRAY = xt::cast<bool>(xt::random::randint<uint16_t>({NUM_BIN_ENTRIES}, 0, 2));

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
        ASSERT_EQ(recon_obj.getMaskArray(), MASK_ARRAY);
        ASSERT_FALSE(recon_obj.getFirstVal());
        ASSERT_FALSE(recon_obj.getRLEntries().has_value());

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // TransformID 1
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_1;
        auto NUM_RL_ENTRIES = 10u;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint8_t>(1, MAX_VAL, NUM_RL_ENTRIES);
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.setTransformID(TRANSFORM_ID);
        orig_obj.setFirstVal(FIRST_VAL);
        orig_obj.setRLEntries(orig_rl_entries);

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
        ASSERT_FALSE(recon_obj.getMaskArray().has_value());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        ASSERT_EQ(recon_obj.getRLEntries(), RL_ENTRIES);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // TransformID 2
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_2;
        auto NUM_RL_ENTRIES = 10u;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint8_t>(1, MAX_VAL, NUM_RL_ENTRIES);
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.setTransformID(TRANSFORM_ID);
        orig_obj.setFirstVal(FIRST_VAL);
        orig_obj.setRLEntries(orig_rl_entries);

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
        ASSERT_FALSE(recon_obj.getMaskArray().has_value());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        ASSERT_EQ(recon_obj.getRLEntries(), RL_ENTRIES);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

    // TransformID 3
    {
        auto TRANSFORM_ID = genie::contact::TransformID::ID_3;
        auto NUM_RL_ENTRIES = 10u;
        auto MAX_VAL = (1u <<  (static_cast<uint8_t>(TRANSFORM_ID)*8) )-1;
        genie::contact::UIntVecDtype RL_ENTRIES = xt::linspace<uint8_t>(1, MAX_VAL, NUM_RL_ENTRIES);
        auto NUM_BIN_ENTRIES = static_cast<uint32_t>(xt::sum(RL_ENTRIES)(0));
        auto FIRST_VAL = true;

        auto orig_rl_entries = genie::contact::UIntVecDtype(RL_ENTRIES);

        auto orig_obj = genie::contact::SubcontactMatrixMaskPayload();
        orig_obj.setTransformID(TRANSFORM_ID);
        orig_obj.setFirstVal(FIRST_VAL);
        orig_obj.setRLEntries(orig_rl_entries);

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
        ASSERT_FALSE(recon_obj.getMaskArray().has_value());
        ASSERT_EQ(recon_obj.getFirstVal(), FIRST_VAL);
        ASSERT_EQ(recon_obj.getRLEntries(), RL_ENTRIES);

        ASSERT_TRUE(orig_obj == recon_obj);
    }

}

// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <vector>
#include <sstream>
#include "genie/likelihood/likelihood_payload.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(LikelihoodStructure, RoundTrip_LikelihoodPayload) {
    uint32_t nrows = 10;
    uint32_t ncols = 5;
    std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
    std::vector<uint8_t> additionalPayload = {6, 7};
    bool transform_flag = true;

    genie::likelihood::LikelihoodParameters params; 
    
    genie::likelihood::LikelihoodPayload inputPayload(params, nrows, ncols, payload, additionalPayload);
    inputPayload.setTransformFlag(transform_flag); 

    std::stringstream ss;
    genie::util::BitWriter writer(ss);
    inputPayload.write(writer);
    writer.FlushBits();

    genie::util::BitReader reader(ss);
    genie::likelihood::LikelihoodPayload outputPayload(params, 0, 0, {}, {}); 
    outputPayload.setTransformFlag(transform_flag);
    outputPayload.read(reader);

    ASSERT_EQ(outputPayload.getNRows(), nrows);
    ASSERT_EQ(outputPayload.getNCols(), ncols);
    ASSERT_EQ(outputPayload.getPayload(), payload);
    ASSERT_EQ(outputPayload.getAdditionalPayload(), additionalPayload);
}

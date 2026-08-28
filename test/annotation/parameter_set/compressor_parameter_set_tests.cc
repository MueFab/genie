/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "random_record_fill_in.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class CompressorParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    CompressorParameterSetTests() = default;

    ~CompressorParameterSetTests() override = default;

    void SetUp() override {
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_F(CompressorParameterSetTests, CompressorParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet;

    EXPECT_EQ(compressorParameterSet.getCompressorID(), 0);
    EXPECT_EQ(compressorParameterSet.getNumberOfCompressorSteps(), 0);
    EXPECT_EQ(compressorParameterSet.getAlgorithmIDs().size(), 0);
    EXPECT_EQ(compressorParameterSet.getCompressorStepIDs().size(), 0);
    EXPECT_EQ(compressorParameterSet.IsDefaultParsUsed().size(), 0);
    EXPECT_EQ(compressorParameterSet.getAlgorithmParameters().size(), 0);
}

TEST_F(CompressorParameterSetTests, CompressorParameterSetValues) {  // NOLINT(cert-err58-cpp)
    uint8_t compressor_ID = 2;
    uint8_t n_compressor_steps = 3;
    std::vector<uint8_t> compressor_step_ID{0, 1, 2};
    std::vector<genie::core::AlgoID> algorithm_ID{
        genie::core::AlgoID::BSC, genie::core::AlgoID::BSC, genie::core::AlgoID::BSC};
    std::vector<bool> use_default_pars{false, false, false};
    std::vector<genie::core::parameter::annotation::AlgorithmParameters> algorithm_parameters(n_compressor_steps);
    std::vector<uint8_t> n_in_vars{1, 1, 1};
    std::vector<std::vector<uint8_t>> in_var_ID{{0}, {1}, {2}};
    std::vector<std::vector<uint8_t>> prev_step_ID{{0}, {1}, {2}};
    std::vector<std::vector<uint8_t>> prev_out_var_ID{{0}, {1}, {2}};
    std::vector<uint8_t> n_completed_out_vars{1, 1, 1};
    std::vector<std::vector<uint8_t>> completed_out_var_ID{{0}, {1}, {2}};
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

TEST_F(CompressorParameterSetTests, CompressorParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSet;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSetCheck;
    genie::core::parameter::annotation::CompressorParameterSet compressorParameterSetCheck2;

    compressorParameterSet = RandomContactMatrixParameters.randomCompressorParameterSet();

    std::stringstream InOut;
    genie::util::BitWriter strwriter(InOut);
    genie::util::BitReader strreader(InOut);
    compressorParameterSet.write(strwriter);
    compressorParameterSet.write(strwriter);
    strwriter.FlushBits();
    compressorParameterSetCheck.read(strreader);
    compressorParameterSetCheck2.read(strreader);

    EXPECT_EQ(compressorParameterSet.getCompressorID(), compressorParameterSetCheck.getCompressorID());
    EXPECT_EQ(compressorParameterSet.getCompressorID(), compressorParameterSetCheck2.getCompressorID());
    EXPECT_EQ(compressorParameterSet.getNumberOfCompressorSteps(),
              compressorParameterSetCheck.getNumberOfCompressorSteps());
    EXPECT_EQ(compressorParameterSet.getCompressorStepIDs(), compressorParameterSetCheck.getCompressorStepIDs());
    EXPECT_EQ(compressorParameterSet.getAlgorithmIDs(), compressorParameterSetCheck.getAlgorithmIDs());
    EXPECT_EQ(compressorParameterSet.IsDefaultParsUsed(), compressorParameterSetCheck.IsDefaultParsUsed());
    EXPECT_EQ(compressorParameterSet.getAlgorithmParameters().size(),
              compressorParameterSetCheck.getAlgorithmParameters().size());
}

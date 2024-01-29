/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "Compressors.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

// ---------------------------------------------------------------------------------------------------------------------

Compressor::Compressor(uint8_t compressorID) : selectedCompressorID(compressorID) { initCompressors(); }

void Compressor::compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID = 0) {
    UTILS_DIE_IF(compressorID >= compressorParameters.size(), "not that much compressors defined");
    selectedCompressorID = compressorID;

    std::stringstream intermediateIn;
    std::stringstream intermediateOut;

    auto& comp = compressorParameters.at(selectedCompressorID);

    for (uint8_t i = 0; i < comp.getNumberOfCompressorSteps(); ++i) {
        intermediateOut.str("");
        intermediateOut.clear();
        if (i == 0)
            intermediateIn << input.rdbuf();
        else
            intermediateIn << intermediateOut.rdbuf();
        switch (comp.getAlgorithmIDs().at(i)) {
            case genie::core::AlgoID::BSC: {
                genie::entropy::bsc::BSCParameters parameters;
                genie::entropy::bsc::BSCEncoder encoder;
                encoder.encode(intermediateIn, intermediateOut);
                break;
            }
            case genie::core::AlgoID::LZMA: {
                genie::entropy::lzma::LZMAEncoder encoder;
                encoder.encode(intermediateIn, intermediateOut);
                break;
            }
            case genie::core::AlgoID::ZSTD: {
                genie::entropy::zstd::ZSTDEncoder encoder;
                encoder.encode(intermediateIn, intermediateOut);
                break;
            }
            default:
                break;
        }
        intermediateIn.str("");
        intermediateIn.clear();
    }
    output << intermediateOut.rdbuf();
}

void Compressor::initCompressors() {
    uint8_t compressor_ID = 0;
    {
        uint8_t n_compressor_steps = 1;

        std::vector<uint8_t> compressor_step_ID(n_compressor_steps);
        std::vector<genie::core::AlgoID> algorithm_ID(n_compressor_steps);
        std::vector<bool> use_default_pars(n_compressor_steps);
        std::vector<AlgorithmParameters> algorithm_parameters;  // (n_compressor_steps);

        std::vector<uint8_t> n_in_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);

        std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);

        for (uint8_t i = 0u; i < n_compressor_steps; ++i) {
            compressor_step_ID.at(i) = i;
            algorithm_ID.at(i) = genie::core::AlgoID::BSC;
            use_default_pars.at(i) = true;
            algorithm_parameters.emplace_back(AlgorithmParameters());
            n_in_vars.at(i) = 0;
            n_completed_out_vars.at(i) = 0;
        }
        compressorParameters.emplace_back(CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
            n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID));
    }

    compressor_ID++;
    {
        uint8_t n_compressor_steps = 1;

        std::vector<uint8_t> compressor_step_ID(n_compressor_steps);
        std::vector<genie::core::AlgoID> algorithm_ID(n_compressor_steps);
        std::vector<bool> use_default_pars(n_compressor_steps);
        std::vector<AlgorithmParameters> algorithm_parameters;  // (n_compressor_steps);
        std::vector<uint8_t> n_in_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);
        std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);

        for (uint8_t i = 0; i < n_compressor_steps; ++i) {
            compressor_step_ID.at(i) = i;
            algorithm_ID.at(i) = genie::core::AlgoID::LZMA;
            use_default_pars.at(i) = true;
            algorithm_parameters.emplace_back(AlgorithmParameters());
            n_in_vars.at(i) = 0;
            n_completed_out_vars.at(i) = 0;
        }
        compressorParameters.emplace_back(CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
            n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID));
    }

    compressor_ID++;
    {
        uint8_t n_compressor_steps = 1;

        std::vector<uint8_t> compressor_step_ID(n_compressor_steps);
        std::vector<genie::core::AlgoID> algorithm_ID(n_compressor_steps);
        std::vector<bool> use_default_pars(n_compressor_steps);
        std::vector<AlgorithmParameters> algorithm_parameters;

        std::vector<uint8_t> n_in_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);

        std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);

        for (uint8_t i = 0; i < n_compressor_steps; ++i) {
            compressor_step_ID.at(i) = i;
            algorithm_ID.at(i) = genie::core::AlgoID::JBIG;
            use_default_pars.at(i) = true;
            algorithm_parameters.emplace_back(AlgorithmParameters());
            n_in_vars.at(i) = 0;
            n_completed_out_vars.at(i) = 0;
        }
        compressorParameters.emplace_back(CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
            n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID));
    }
    compressor_ID++;
    {
        uint8_t n_compressor_steps = 2;

        std::vector<uint8_t> compressor_step_ID(n_compressor_steps);
        std::vector<genie::core::AlgoID> algorithm_ID(n_compressor_steps);
        std::vector<bool> use_default_pars(n_compressor_steps);
        std::vector<AlgorithmParameters> algorithm_parameters;

        std::vector<uint8_t> n_in_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
        std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);

        std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
        std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);

        algorithm_ID.at(0) = genie::core::AlgoID::BSC;
        algorithm_ID.at(1) = genie::core::AlgoID::LZMA;
        for (uint8_t i = 0; i < n_compressor_steps; ++i) {
            compressor_step_ID.at(i) = i;
            use_default_pars.at(i) = true;
            algorithm_parameters.emplace_back(AlgorithmParameters());
            n_in_vars.at(i) = 0;
            n_completed_out_vars.at(i) = 0;
        }
        compressorParameters.emplace_back(CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
            n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID));
    }
}

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

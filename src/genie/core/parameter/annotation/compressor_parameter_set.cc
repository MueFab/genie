/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/parameter/annotation/compressor_parameter_set.h"

#include <vector>

#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::annotation {

CompressorParameterSet::CompressorParameterSet() : compressor_ID(1), compressorSteps{} {}

CompressorParameterSet::CompressorParameterSet(util::BitReader& reader) { read(reader); }

CompressorParameterSet::CompressorParameterSet(
    uint8_t compressor_ID, uint8_t n_compressor_steps, std::vector<uint8_t> compressor_step_ID,
    std::vector<genie::core::AlgoID> algorithm_ID, std::vector<bool> use_default_pars,
    std::vector<AlgorithmParameters> algorithm_parameters, std::vector<uint8_t> n_in_vars,
    std::vector<std::vector<uint8_t>> in_var_ID, std::vector<std::vector<uint8_t>> prev_step_ID,
    std::vector<std::vector<uint8_t>> prev_out_var_ID, std::vector<uint8_t> n_completed_out_vars,
    std::vector<std::vector<uint8_t>> completed_out_var_ID)
    : compressor_ID(compressor_ID) {
    compressorSteps.resize(n_compressor_steps);
    size_t algParindex = 0;
    for (uint8_t idx_i = 0; idx_i < n_compressor_steps; ++idx_i) {
        compressorSteps.at(idx_i).stepID = compressor_step_ID.at(idx_i);
        compressorSteps.at(idx_i).algorithmID = algorithm_ID.at(idx_i);
        compressorSteps.at(idx_i).useDefaultAlgorithmParameters = use_default_pars.at(idx_i);
        if (!use_default_pars.at(idx_i))
            compressorSteps.at(idx_i).algorithm_parameters = algorithm_parameters.at(algParindex++);
        compressorSteps.at(idx_i).in_var_ID.resize(n_in_vars.at(idx_i));
        compressorSteps.at(idx_i).prev_step_ID.resize(n_in_vars.at(idx_i));
        compressorSteps.at(idx_i).prev_out_var_ID.resize(n_in_vars.at(idx_i));
        for (uint8_t idx_j = 0; idx_j < n_in_vars.at(idx_i); ++idx_j) {
            compressorSteps.at(idx_i).in_var_ID.at(idx_j) = in_var_ID.at(idx_i).at(idx_j);
            compressorSteps.at(idx_i).prev_step_ID.at(idx_j) = prev_step_ID.at(idx_i).at(idx_j);
            compressorSteps.at(idx_i).prev_out_var_ID.at(idx_j) = prev_out_var_ID.at(idx_i).at(idx_j);
        }
        compressorSteps.at(idx_i).completed_out_var_ID.resize(n_completed_out_vars.at(idx_i));
        for (auto idx_j = 0; idx_j < n_completed_out_vars.at(idx_i); ++idx_j)
            compressorSteps.at(idx_i).completed_out_var_ID.at(idx_j) = completed_out_var_ID.at(idx_i).at(idx_j);
    }
}

void CompressorParameterSet::read(util::BitReader& reader) {
    compressor_ID = static_cast<uint8_t>(reader.ReadBits(8));
    uint8_t n_compressor_steps = static_cast<uint8_t>(reader.ReadBits(4));
    compressorSteps.resize(n_compressor_steps);
    for (auto idx_i = 0; idx_i < n_compressor_steps; ++idx_i) {
        compressorSteps.at(idx_i).stepID = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(idx_i).algorithmID = static_cast<genie::core::AlgoID>(reader.ReadBits(5));
        compressorSteps.at(idx_i).useDefaultAlgorithmParameters = static_cast<bool>(reader.ReadBits(1));

        if (!compressorSteps.at(idx_i).useDefaultAlgorithmParameters)
            compressorSteps.at(idx_i).algorithm_parameters.read(reader);

        uint8_t nrOfInVars = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(idx_i).in_var_ID.resize(nrOfInVars);
        compressorSteps.at(idx_i).prev_step_ID.resize(nrOfInVars);
        compressorSteps.at(idx_i).prev_out_var_ID.resize(nrOfInVars);
        for (auto idx_j = 0; idx_j < nrOfInVars; ++idx_j) {
            compressorSteps.at(idx_i).in_var_ID.at(idx_j) = static_cast<uint8_t>(reader.ReadBits(4));
            compressorSteps.at(idx_i).prev_step_ID.at(idx_j) = static_cast<uint8_t>(reader.ReadBits(4));
            compressorSteps.at(idx_i).prev_out_var_ID.at(idx_j) = static_cast<uint8_t>(reader.ReadBits(4));
        }
        uint8_t nrOfoutVars = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(idx_i).completed_out_var_ID.resize(nrOfoutVars);
        for (auto idx_j = 0; idx_j < nrOfoutVars; ++idx_j)
            compressorSteps.at(idx_i).completed_out_var_ID.at(idx_j) = static_cast<uint8_t>(reader.ReadBits(4));
    }
    reader.FlushHeldBits();
}

void CompressorParameterSet::write(util::BitWriter& writer) const {
    writer.WriteBits(compressor_ID, 8);
    writer.WriteBits(compressorSteps.size(), 4);
    for (auto step : compressorSteps) {
        writer.WriteBits(step.stepID, 4);
        writer.WriteBits(static_cast<uint8_t>(step.algorithmID), 5);
        writer.WriteBits(step.useDefaultAlgorithmParameters, 1);
        if (!step.useDefaultAlgorithmParameters) step.algorithm_parameters.write(writer);
        writer.WriteBits(step.in_var_ID.size(), 4);
        for (size_t idx_i = 0; idx_i < step.in_var_ID.size(); ++idx_i) {
            writer.WriteBits(step.in_var_ID.at(idx_i), 4);
            writer.WriteBits(step.prev_step_ID.at(idx_i), 4);
            writer.WriteBits(step.prev_out_var_ID.at(idx_i), 4);
        }
        writer.WriteBits(step.completed_out_var_ID.size(), 4);
        for (auto outvar : step.completed_out_var_ID) writer.WriteBits(outvar, 4);
    }
    /*
    writer.WriteBits(n_compressor_steps, 4);
    uint8_t algorithm_index = 0;
    for (auto idx_i = 0; idx_i < n_compressor_steps; ++idx_i) {
        writer.WriteBits(compressor_step_ID[idx_i], 4);
        writer.WriteBits(static_cast<uint8_t>(algorithm_ID[idx_i]), 5);
        writer.WriteBits(use_default_pars[idx_i], 1);
        if (!use_default_pars[idx_i]) {
            (algorithm_parameters[algorithm_index].write(writer));
            algorithm_index++;
        }
        writer.WriteBits(n_in_vars[idx_i], 4);
        for (auto idx_j = 0; idx_j < n_in_vars[idx_i]; ++idx_j) {
            writer.WriteBits(in_var_ID[idx_i][idx_j], 4);
            writer.WriteBits(prev_step_ID[idx_i][idx_j], 4);
            writer.WriteBits(prev_out_var_ID[idx_i][idx_j], 4);
        }
        writer.WriteBits(n_completed_out_vars[idx_i], 4);
        for (auto idx_j = 0; idx_j < n_completed_out_vars[idx_i]; ++idx_j) writer.WriteBits(completed_out_var_ID[idx_i][idx_j], 4);
    }
    */
    writer.FlushBits();
}

void CompressorParameterSet::addCompressorStep(compressorStep stepParameters) {
    for (auto step : compressorSteps)
        UTILS_DIE_IF(stepParameters.stepID == step.stepID, "compressor step ID already defined");
    compressorSteps.push_back(stepParameters);
}

size_t CompressorParameterSet::getSize(util::BitWriter& writesize) const {
    write(writesize);
    return writesize.GetTotalBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter::annotation

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

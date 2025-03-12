/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "CompressorParameterSet.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

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
    for (uint8_t i = 0; i < n_compressor_steps; ++i) {
        compressorSteps.at(i).stepID = compressor_step_ID.at(i);
        compressorSteps.at(i).algorithmID = algorithm_ID.at(i);
        compressorSteps.at(i).useDefaultAlgorithmParameters = use_default_pars.at(i);
        if (!use_default_pars.at(i))
            compressorSteps.at(i).algorithm_parameters = algorithm_parameters.at(algParindex++);
        compressorSteps.at(i).in_var_ID.resize(n_in_vars.at(i));
        compressorSteps.at(i).prev_step_ID.resize(n_in_vars.at(i));
        compressorSteps.at(i).prev_out_var_ID.resize(n_in_vars.at(i));
        for (uint8_t j = 0; j < n_in_vars.at(i); ++j) {
            compressorSteps.at(i).in_var_ID.at(j) = in_var_ID.at(i).at(j);
            compressorSteps.at(i).prev_step_ID.at(j) = prev_step_ID.at(i).at(j);
            compressorSteps.at(i).prev_out_var_ID.at(j) = prev_out_var_ID.at(i).at(j);
        }
        compressorSteps.at(i).completed_out_var_ID.resize(n_completed_out_vars.at(i));
        for (auto j = 0; j < n_completed_out_vars.at(i); ++j)
            compressorSteps.at(i).completed_out_var_ID.at(j) = completed_out_var_ID.at(i).at(j);
    }
}

void CompressorParameterSet::read(util::BitReader& reader) {
    compressor_ID = static_cast<uint8_t>(reader.ReadBits(8));
    uint8_t n_compressor_steps = static_cast<uint8_t>(reader.ReadBits(4));
    compressorSteps.resize(n_compressor_steps);
    for (auto i = 0; i < n_compressor_steps; ++i) {
        compressorSteps.at(i).stepID = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(i).algorithmID = static_cast<genie::core::AlgoID>(reader.ReadBits(5));
        compressorSteps.at(i).useDefaultAlgorithmParameters = static_cast<bool>(reader.ReadBits(1));

        if (!compressorSteps.at(i).useDefaultAlgorithmParameters)
            compressorSteps.at(i).algorithm_parameters.read(reader);

        uint8_t nrOfInVars = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(i).in_var_ID.resize(nrOfInVars);
        compressorSteps.at(i).prev_step_ID.resize(nrOfInVars);
        compressorSteps.at(i).prev_out_var_ID.resize(nrOfInVars);
        for (auto j = 0; j < nrOfInVars; ++j) {
            compressorSteps.at(i).in_var_ID.at(j) = static_cast<uint8_t>(reader.ReadBits(4));
            compressorSteps.at(i).prev_step_ID.at(j) = static_cast<uint8_t>(reader.ReadBits(4));
            compressorSteps.at(i).prev_out_var_ID.at(j) = static_cast<uint8_t>(reader.ReadBits(4));
        }
        uint8_t nrOfoutVars = static_cast<uint8_t>(reader.ReadBits(4));
        compressorSteps.at(i).completed_out_var_ID.resize(nrOfoutVars);
        for (auto j = 0; j < nrOfoutVars; ++j)
            compressorSteps.at(i).completed_out_var_ID.at(j) = static_cast<uint8_t>(reader.ReadBits(4));
    }
    reader.FlushHeldBits();
}

void CompressorParameterSet::write(core::Writer& writer) const {
  writer.Write(compressor_ID, 8);
    writer.Write(compressorSteps.size(), 4);
    for (auto step : compressorSteps) {
      writer.Write(step.stepID, 4);
        writer.Write(static_cast<uint8_t>(step.algorithmID), 5);
        writer.Write(step.useDefaultAlgorithmParameters, 1);
        if (!step.useDefaultAlgorithmParameters) step.algorithm_parameters.write(writer);
        writer.Write(step.in_var_ID.size(), 4);
        for (size_t i = 0; i < step.in_var_ID.size(); ++i) {
          writer.Write(step.in_var_ID.at(i), 4);
            writer.Write(step.prev_step_ID.at(i), 4);
            writer.Write(step.prev_out_var_ID.at(i), 4);
        }
        writer.Write(step.completed_out_var_ID.size(), 4);
        for (auto outvar : step.completed_out_var_ID) writer.Write(outvar, 4);
    }
    /*
    writer.write(n_compressor_steps, 4);
    uint8_t algorithm_index = 0;
    for (auto i = 0; i < n_compressor_steps; ++i) {
        writer.write(compressor_step_ID[i], 4);
        writer.write(static_cast<uint8_t>(algorithm_ID[i]), 5);
        writer.write(use_default_pars[i], 1);
        if (!use_default_pars[i]) {
            (algorithm_parameters[algorithm_index].write(writer));
            algorithm_index++;
        }
        writer.write(n_in_vars[i], 4);
        for (auto j = 0; j < n_in_vars[i]; ++j) {
            writer.write(in_var_ID[i][j], 4);
            writer.write(prev_step_ID[i][j], 4);
            writer.write(prev_out_var_ID[i][j], 4);
        }
        writer.write(n_completed_out_vars[i], 4);
        for (auto j = 0; j < n_completed_out_vars[i]; ++j) writer.write(completed_out_var_ID[i][j], 4);
    }
    */
    writer.Flush();
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
    for (size_t i = 0; i < step.in_var_ID.size(); ++i) {
      writer.WriteBits(step.in_var_ID.at(i), 4);
      writer.WriteBits(step.prev_step_ID.at(i), 4);
      writer.WriteBits(step.prev_out_var_ID.at(i), 4);
    }
    writer.WriteBits(step.completed_out_var_ID.size(), 4);
    for (auto outvar : step.completed_out_var_ID) writer.WriteBits(outvar, 4);
  }
  /*
  writer.WriteBits(n_compressor_steps, 4);
  uint8_t algorithm_index = 0;
  for (auto i = 0; i < n_compressor_steps; ++i) {
      writer.WriteBits(compressor_step_ID[i], 4);
      writer.WriteBits(static_cast<uint8_t>(algorithm_ID[i]), 5);
      writer.WriteBits(use_default_pars[i], 1);
      if (!use_default_pars[i]) {
          (algorithm_parameters[algorithm_index].write(writer));
          algorithm_index++;
      }
      writer.WriteBits(n_in_vars[i], 4);
      for (auto j = 0; j < n_in_vars[i]; ++j) {
          writer.WriteBits(in_var_ID[i][j], 4);
          writer.WriteBits(prev_step_ID[i][j], 4);
          writer.WriteBits(prev_out_var_ID[i][j], 4);
      }
      writer.WriteBits(n_completed_out_vars[i], 4);
      for (auto j = 0; j < n_completed_out_vars[i]; ++j) writer.WriteBits(completed_out_var_ID[i][j], 4);
  }
  */
  writer.FlushBits();
}

void CompressorParameterSet::addCompressorStep(compressorStep stepParameters) {
    for (auto step : compressorSteps)
        UTILS_DIE_IF(stepParameters.stepID == step.stepID, "compressor step ID already defined");
    compressorSteps.push_back(stepParameters);
}

size_t CompressorParameterSet::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.GetBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

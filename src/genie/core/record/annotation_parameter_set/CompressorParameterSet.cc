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
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "CompressorParameterSet.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

CompressorParameterSet::CompressorParameterSet()
    : compressor_ID(1),
      n_compressor_steps(0),
      compressor_step_ID{},
      algorithm_ID{},
      use_default_pars{},
      algorithm_parameters{},
      n_in_vars{},
      in_var_ID{},
      prev_step_ID{},
      prev_out_var_ID{},
      n_completed_out_vars{},
      completed_out_var_ID{} {}

CompressorParameterSet::CompressorParameterSet(util::BitReader& reader) { read(reader); }

CompressorParameterSet::CompressorParameterSet(
    uint8_t compressor_ID, uint8_t n_compressor_steps, std::vector<uint8_t> compressor_step_ID,
    std::vector<genie::core::AlgoID> algorithm_ID, std::vector<bool> use_default_pars,
    std::vector<AlgorithmParameters> algorithm_parameters, std::vector<uint8_t> n_in_vars,
    std::vector<std::vector<uint8_t>> in_var_ID, std::vector<std::vector<uint8_t>> prev_step_ID,
    std::vector<std::vector<uint8_t>> prev_out_var_ID, std::vector<uint8_t> n_completed_out_vars,
    std::vector<std::vector<uint8_t>> completed_out_var_ID)
    : compressor_ID(compressor_ID),
      n_compressor_steps(n_compressor_steps),
      compressor_step_ID(compressor_step_ID),
      algorithm_ID(algorithm_ID),
      use_default_pars(use_default_pars),
      algorithm_parameters(algorithm_parameters),
      n_in_vars(n_in_vars),
      in_var_ID(in_var_ID),
      prev_step_ID(prev_step_ID),
      prev_out_var_ID(prev_out_var_ID),
      n_completed_out_vars(n_completed_out_vars),
      completed_out_var_ID(completed_out_var_ID) {}

void CompressorParameterSet::read(util::BitReader& reader) {
    compressor_ID = static_cast<uint8_t>(reader.read_b(8));
    n_compressor_steps = static_cast<uint8_t>(reader.read_b(4));

    use_default_pars.resize(n_compressor_steps);

    n_in_vars.resize(n_compressor_steps);
    in_var_ID.resize(n_compressor_steps);
    prev_step_ID.resize(n_compressor_steps);
    prev_out_var_ID.resize(n_compressor_steps);

    n_completed_out_vars.resize(n_compressor_steps);
    completed_out_var_ID.resize(n_compressor_steps);

    for (auto i = 0; i < n_compressor_steps; ++i) {
        compressor_step_ID.push_back(static_cast<uint8_t>(reader.read_b(4)));
        algorithm_ID.push_back(static_cast<genie::core::AlgoID>(reader.read_b(5)));
        use_default_pars[i] = (static_cast<bool>(reader.read_b(1)));
        if (!use_default_pars[i]) {
            AlgorithmParameters algorithmParameters(reader);
            algorithm_parameters.push_back(algorithmParameters);
        }
        n_in_vars[i] = static_cast<uint8_t>(reader.read_b(4));
        for (auto j = 0; j < n_in_vars[i]; ++j) {
            in_var_ID[i].push_back(static_cast<uint8_t>(reader.read_b(4)));
            prev_step_ID[i].push_back(static_cast<uint8_t>(reader.read_b(4)));
            prev_out_var_ID[i].push_back(static_cast<uint8_t>(reader.read_b(4)));
        }
        n_completed_out_vars[i] = static_cast<uint8_t>(reader.read_b(4));
        for (auto j = 0; j < n_completed_out_vars[i]; ++j)
            completed_out_var_ID[i].push_back(static_cast<uint8_t>(reader.read_b(4)));
    }
    reader.flush();
}

void CompressorParameterSet::write(core::Writer& writer) const {
    writer.write(compressor_ID, 8);
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
    writer.flush();
}

size_t CompressorParameterSet::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

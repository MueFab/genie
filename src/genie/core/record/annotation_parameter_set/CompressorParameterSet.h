/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORPARAMETERSET_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/writer.h"

#include "AlgorithmParameters.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

class CompressorParameterSet {
 private:
    uint8_t compressor_ID;
    uint8_t n_compressor_steps;
    std::vector<uint8_t> compressor_step_ID;
    std::vector<uint8_t> algorithm_ID;
    std::vector<bool> use_default_pars;
    std::vector<AlgorithmParameters> algorithm_parameters;
    std::vector<uint8_t> n_in_vars;
    std::vector<std::vector<uint8_t>> in_var_ID;
    std::vector<std::vector<uint8_t>> prev_step_ID;
    std::vector<std::vector<uint8_t>> prev_out_var_ID;
    std::vector<uint8_t> n_completed_out_vars;
    std::vector<std::vector<uint8_t>> completed_out_var_ID;

 public:
    CompressorParameterSet();
    explicit CompressorParameterSet(util::BitReader& reader);
    CompressorParameterSet(uint8_t compressor_ID, uint8_t n_compressor_steps, std::vector<uint8_t> compressor_step_ID,
                           std::vector<uint8_t> algorithm_ID, std::vector<bool> use_default_pars,
                           std::vector<AlgorithmParameters> algorithm_parameters, std::vector<uint8_t> n_in_vars,
                           std::vector<std::vector<uint8_t>> in_var_ID, std::vector<std::vector<uint8_t>> prev_step_ID,
                           std::vector<std::vector<uint8_t>> prev_out_var_ID, std::vector<uint8_t> n_completed_out_vars,
                           std::vector<std::vector<uint8_t>> completed_out_var_ID);

    void read(util::BitReader& reader);
    void write(core::Writer& writer) const;

    size_t getSize(core::Writer& writesize) const;

    uint8_t getCompressorID() const { return compressor_ID; }
    uint8_t getNumberOfCompressorSteps() const { return n_compressor_steps; }
    std::vector<uint8_t> getCompressorStepIDs() const { return compressor_step_ID; }
    std::vector<uint8_t> getAlgorithmIDs() const { return algorithm_ID; }
    std::vector<bool> IsDefaultParsUsed() const { return use_default_pars; }
    std::vector<AlgorithmParameters> getAlgorithmParameters() const { return algorithm_parameters; }
    std::vector<uint8_t> getNumberInVars() const { return n_in_vars; }
    std::vector<std::vector<uint8_t>> getInVarsIDs() const { return in_var_ID; }
    std::vector<std::vector<uint8_t>> getPreviousStepIDs() const { return prev_step_ID; }
    std::vector<std::vector<uint8_t>> getPreviousOutVarIDs() const { return prev_out_var_ID; }
    std::vector<uint8_t> getNumberOfCompletedOutVars() const { return n_completed_out_vars; }
    std::vector<std::vector<uint8_t>> getCompetedOutVarIDs() const { return completed_out_var_ID; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_COMPRESSORPARAMETERSET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

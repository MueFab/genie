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
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AlgorithmParameters.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

struct compressorStep {
    uint8_t stepID{0};
    genie::core::AlgoID algorithmID{genie::core::AlgoID::BSC};
    bool useDefaultAlgorithmParameters{true};
    AlgorithmParameters algorithm_parameters;
    std::vector<uint8_t> in_var_ID{};
    std::vector<uint8_t> prev_step_ID{};
    std::vector<uint8_t> prev_out_var_ID{};
    std::vector<uint8_t> completed_out_var_ID{};
};

class CompressorParameterSet {
 private:
    uint8_t compressor_ID;
    std::vector<compressorStep> compressorSteps;

 public:
    CompressorParameterSet();
    explicit CompressorParameterSet(util::BitReader& reader);
    explicit CompressorParameterSet(uint8_t compressorID) : compressor_ID(compressorID), compressorSteps{} {}

    CompressorParameterSet(uint8_t compressor_ID, uint8_t n_compressor_steps, std::vector<uint8_t> compressor_step_ID,
                           std::vector<genie::core::AlgoID> algorithm_ID, std::vector<bool> use_default_pars,
                           std::vector<AlgorithmParameters> algorithm_parameters, std::vector<uint8_t> n_in_vars,
                           std::vector<std::vector<uint8_t>> in_var_ID, std::vector<std::vector<uint8_t>> prev_step_ID,
                           std::vector<std::vector<uint8_t>> prev_out_var_ID, std::vector<uint8_t> n_completed_out_vars,
                           std::vector<std::vector<uint8_t>> completed_out_var_ID);

    void addCompressorStep(compressorStep stepParameters);

    void read(util::BitReader& reader);
    void write(core::Writer& writer) const;

    size_t getSize(core::Writer& writesize) const;

    uint8_t getCompressorID() const { return compressor_ID; }
    uint8_t getNumberOfCompressorSteps() const { return static_cast<uint8_t>(compressorSteps.size()); }
    std::vector<uint8_t> getCompressorStepIDs() const {
        std::vector<uint8_t> allStepIDs;
        for (auto step : compressorSteps) allStepIDs.push_back(step.stepID);
        return allStepIDs;
    }
    std::vector<genie::core::AlgoID> getAlgorithmIDs() const {
        std::vector<genie::core::AlgoID> allAlgIDs;
        for (auto step : compressorSteps) allAlgIDs.push_back(step.algorithmID);
        return allAlgIDs;
    }
    std::vector<bool> IsDefaultParsUsed() const {
        std::vector<bool> alldefaultAlg;
        for (auto step : compressorSteps) alldefaultAlg.push_back(step.useDefaultAlgorithmParameters);
        return alldefaultAlg;
    }
    std::vector<AlgorithmParameters> getAlgorithmParameters() const {
        std::vector<AlgorithmParameters> algPar;
        for (auto step : compressorSteps)
            if (!step.useDefaultAlgorithmParameters) algPar.push_back(step.algorithm_parameters);
        return algPar;
    }
    std::vector<uint8_t> getNumberInVars() const {
        std::vector<uint8_t> nrInvars;
        for (auto step : compressorSteps) nrInvars.push_back(static_cast<uint8_t>(step.in_var_ID.size()));
        return nrInvars;
    }
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

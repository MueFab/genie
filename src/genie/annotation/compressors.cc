/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/annotation/compressors.h"

#include <cctype>
#include <cstdint>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/entropy/ser/encoder.h"
#include "genie/entropy/factory/encoder_factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

// --- Helper functions for reading parameters (not part of CompressorParser class) ---

static entropy::lzma::LZMAParameters readLzmaParameters(std::vector<std::string>& stringpars) {
  entropy::lzma::LZMAParameters pars;
  if (stringpars.empty())
    return pars;

  CompressorParser::Parameter parameter;
  size_t index = 0;

  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.level);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.dictSize);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.lc);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.lp);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.pb);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.fb);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.numThreads);

  stringpars.clear();
  return pars;
}

static entropy::bsc::BSCParameters readBscParameters(std::vector<std::string>& stringpars) {
  entropy::bsc::BSCParameters pars;
  if (stringpars.empty())
    return pars;

  CompressorParser::Parameter parameter;
  size_t index = 0;

  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.lzpHashSize);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.lzpMinLen);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.blockSorter);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.coder);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.features);

  stringpars.clear();
  return pars;
}

static entropy::zstd::ZSTDParameters readZstdParameters(std::vector<std::string>& stringpars) {
  entropy::zstd::ZSTDParameters pars;
  if (stringpars.empty())
    return pars;

  CompressorParser::Parameter parameter;
  size_t index = 0;

  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.use_dictionary_flag);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.dictionary_size);
  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.dictionary);

  stringpars.clear();
  return pars;
}

static entropy::ser::SERParameters readSerParameters(std::vector<std::string>& stringpars) {
  entropy::ser::SERParameters pars;
  if (stringpars.empty())
    return pars;

  CompressorParser::Parameter parameter;
  size_t index = 0;

  if (index < stringpars.size())
    parameter.read(stringpars.at(index++), pars.order);

  stringpars.clear();
  return pars;
}

Compressor::Compressor() : selectedCompressorID(0), compressorParameters{} {}

void Compressor::parseConfig(std::stringstream& config) {
  CompressorParser::parseConfig(*this, config);
}

void Compressor::compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID = 0) {
    UTILS_DIE_IF(compressorID > compressorParameters.size(), "not that much compressors defined");
    selectedCompressorID = compressorID;

    std::stringstream intermediateIn;
    std::stringstream intermediateOut;

    auto& comp = compressorParameters.at(0);
    for (auto& compressor : compressorParameters)
        if (compressor.getCompressorID() == compressorID) comp = compressor;

    for (uint8_t i = 0; i < comp.getNumberOfCompressorSteps(); ++i) {
        if (i == 0) {
            intermediateIn << input.rdbuf();
        } else {
            intermediateIn.str("");
            intermediateIn.clear();
            intermediateIn << intermediateOut.rdbuf();
            intermediateOut.str("");
            intermediateOut.clear();
        }

        switch (comp.getAlgorithmIDs().at(i)) {
            case core::AlgoID::BSC: {
                entropy::bsc::BSCEncoder encoder;
                encoder.encode(intermediateIn, intermediateOut);
                break;
            }
            case core::AlgoID::LZMA: {
                entropy::lzma::LZMAEncoder encoder;
                encoder.encode(intermediateIn, intermediateOut);
                break;
            }
            case core::AlgoID::ZSTD: {
                entropy::zstd::ZSTDEncoder encoder;
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

void Compressor::compress(core::record::annotation_access_unit::TypedData& input,
                          std::vector<core::record::annotation_access_unit::TypedData>& output,
                          uint8_t compressorID) {
  selectedCompressorID = compressorID;
  auto comp = compressorParameters.at(0);
  for (auto& compressor : compressorParameters)
    if (compressor.getCompressorID() == compressorID)
      comp = compressor;
  std::map<uint8_t, std::unique_ptr<entropy::base::Encoder>> encoders;
  auto compressorStepIDs = comp.getCompressorStepIDs();
  bool firstStep = true;
  for (auto stepID : compressorStepIDs) {
    auto& step = comp.getCompressorStep(stepID);
    encoders[stepID] = entropy::factory::EncoderFactory::createEncoder(step.algorithmID,
                                                                       step.algorithm_parameters);
    if (firstStep) {
      encoders[stepID]->setInput(0, input);
      firstStep = false;
    } else {
      for (auto inVarIndex = 0; inVarIndex < step.in_var_ID.size(); inVarIndex++) {
        encoders[stepID]->setInput(
            step.in_var_ID[inVarIndex],
            encoders[step.prev_step_ID[inVarIndex]]->getOutput(step.prev_out_var_ID[inVarIndex]));
      }
    }
    encoders[stepID]->encode();
    for (auto outVarID : comp.getCompressorStep(stepID).completed_out_var_ID) {
      output.emplace_back(encoders[stepID]->getOutput(outVarID));
    }
  }
}

// --- CompressorParser::Parameter Implementation ---

void CompressorParser::Parameter::read(const std::string& parameterIn, uint8_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<uint8_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, bool& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<bool>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, core::AlgoID& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = core::stringToAlgoID(parameterIn);
}

void CompressorParser::Parameter::read(const std::string& parameterIn, uint16_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<uint16_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, uint32_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<uint32_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, uint64_t& parameterOut) {
  if (parameterIn.empty())
    return;
  std::istringstream iss(parameterIn);
  iss >> parameterOut;
}

void CompressorParser::Parameter::read(const std::string& parameterIn, int8_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<int8_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, int16_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<int16_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, int32_t& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = static_cast<int32_t>(std::stoi(parameterIn));
}

void CompressorParser::Parameter::read(const std::string& parameterIn, std::string& parameterOut) {
  if (parameterIn.empty())
    return;
  parameterOut = parameterIn;
}

// --- CompressorParser Static Methods ---

std::vector<std::string> CompressorParser::tokenizeCurlyHashNumber(const std::string& input) {
  std::vector<std::string> tokens;
  size_t i = 0;
  while (i < input.size()) {
    if (std::isspace(static_cast<unsigned char>(input[i]))) {
      ++i;
    } else if (input[i] == '{' || input[i] == '}' || input[i] == '#') {
      tokens.emplace_back(1, input[i]);
      ++i;
    } else if (std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '-') {
      size_t start = i;
      while (i < input.size() &&
             (std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '-'))
        ++i;
      tokens.emplace_back(input.substr(start, i - start));
    } else if (std::isalpha(static_cast<unsigned char>(input[i]))) {
      size_t start = i;
      while (i < input.size() && std::isalpha(static_cast<unsigned char>(input[i])))
        ++i;
      tokens.emplace_back(input.substr(start, i - start));
    } else {
      ++i;
    }
  }
  return tokens;
}

std::vector<std::string> CompressorParser::getAlgoParamsGroup(
    const std::vector<std::string>& tokens, size_t& idx) {
  std::vector<std::string> result;
  if (idx < tokens.size()) {
    if (tokens.at(idx) != "{")
      throw std::runtime_error("Expected '{'");
    ++idx;
    while (idx < tokens.size() && tokens.at(idx) != "}") {
      result.push_back(tokens.at(idx));
      ++idx;
    }
    if (idx == tokens.size() || tokens.at(idx) != "}")
      throw std::runtime_error("Expected '}'");
    ++idx;
  }
  return result;
}

void CompressorParser::parseInVarGroup(const std::vector<std::string>& tokens, size_t& idx,
                                       std::vector<uint8_t>& in_var_ID,
                                       std::vector<uint8_t>& prev_step_ID,
                                       std::vector<uint8_t>& prev_out_var_ID) {
  Parameter parameter;
  if (tokens.at(idx) != "{")
    throw std::runtime_error("Expected '{' for in-var group");
  ++idx;
  while (idx < tokens.size() && tokens.at(idx) == "{") {
    ++idx;
    if (idx + 2 >= tokens.size())
      throw std::runtime_error("Malformed in-var group");
    parameter.read(tokens.at(idx++), in_var_ID.emplace_back());
    parameter.read(tokens.at(idx++), prev_step_ID.emplace_back());
    parameter.read(tokens.at(idx++), prev_out_var_ID.emplace_back());
    if (tokens.at(idx) != "}")
      throw std::runtime_error("Expected '}' after in-var triple");
    ++idx;
  }
  if (tokens.at(idx) != "}")
    throw std::runtime_error("Expected '}' after in-var group");
  ++idx;
}

void CompressorParser::parseCompletedOutVarGroup(const std::vector<std::string>& tokens,
                                                 size_t& idx,
                                                 std::vector<uint8_t>& completed_out_var_ID) {
  Parameter parameter;
  if (tokens.at(idx) != "{")
    throw std::runtime_error("Expected '{' for completed out-var group");
  ++idx;
  while (idx < tokens.size() && tokens.at(idx) != "}") {
    parameter.read(tokens.at(idx++), completed_out_var_ID.emplace_back());
  }
  if (tokens.at(idx) != "}")
    throw std::runtime_error("Expected '}' after completed out-var group");
  ++idx;
}

void CompressorParser::parseConfig(Compressor& compressor, std::stringstream& config) {
  std::string commandline;
  while (getline(config, commandline, '\n')) {
    auto tokens = tokenizeCurlyHashNumber(commandline);
    if (tokens.empty())
      continue;
    if (tokens[0] == "#")
      continue;
    if (tokens[0] == "compressor") {
      parseCompressor(compressor, tokens);
    }
  }
}

void CompressorParser::parseCompressor(Compressor& compressor,
                                       std::vector<std::string> commandline) {
  Parameter parameter;
  uint8_t compressorID = 0;
  parameter.read(commandline.at(1), compressorID);

  if (compressorID == 0) {
    auto it = std::find_if(
        compressor.compressorParameters.begin(), compressor.compressorParameters.end(),
        [compressorID](const auto& compPar) { return compPar.getCompressorID() == compressorID; });
    if (it == compressor.compressorParameters.end()) {
      compressor.compressorParameters.emplace_back(compressorID);
    }
    return;
  }
  core::record::annotation_parameter_set::compressorStep step;
  parameter.read(commandline.at(2), step.stepID);
  parameter.read(commandline.at(3), step.algorithmID);

  size_t index = 4;
  std::vector<std::string> sub = getAlgoParamsGroup(commandline, index);

  switch (step.algorithmID) {
    case core::AlgoID::LZMA: {
      entropy::lzma::LZMAParameters parameters = readLzmaParameters(sub);
      step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
      step.algorithm_parameters = parameters.convertToAlgorithmParameters();
      break;
    }
    case core::AlgoID::ZSTD: {
      entropy::zstd::ZSTDParameters parameters = readZstdParameters(sub);
      step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
      step.algorithm_parameters = parameters.convertToAlgorithmParameters();
      break;
    }
    case core::AlgoID::BSC: {
      entropy::bsc::BSCParameters parameters = readBscParameters(sub);
      step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
      step.algorithm_parameters = parameters.convertToAlgorithmParameters();
      break;
    }
    case core::AlgoID::SER: {
      entropy::ser::SERParameters parameters = readSerParameters(sub);
      step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
      step.algorithm_parameters = parameters.convertToAlgorithmParameters();
      break;
    }
    default:
      break;
  }

  if (index < commandline.size()) {
    std::vector<uint8_t> in_var_ID, prev_step_ID, prev_out_var_ID;
    parseInVarGroup(commandline, index, in_var_ID, prev_step_ID, prev_out_var_ID);
    step.in_var_ID = in_var_ID;
    step.prev_step_ID = prev_step_ID;
    step.prev_out_var_ID = prev_out_var_ID;
  }

  if (index < commandline.size()) {
    std::vector<uint8_t> completed_out_var_ID;
    parseCompletedOutVarGroup(commandline, index, completed_out_var_ID);
    step.completed_out_var_ID = completed_out_var_ID;
  }

  bool added = false;
  for (auto& compPar : compressor.compressorParameters) {
    if (compPar.getCompressorID() == compressorID) {
      compPar.addCompressorStep(step);
      added = true;
    }
  }
  if (!added) {
    compressor.compressorParameters.emplace_back(compressorID);
    compressor.compressorParameters.back().addCompressorStep(step);
  }
}


}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

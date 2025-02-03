/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cctype>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "Compressors.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

Compressor::Compressor() : selectedCompressorID(0), compressorParameters{} {}

void Compressor::parseConfig(std::stringstream& config) {
    UTILS_DIE_IF(config.str().empty(), "compressor config is empty ");
    std::string commandline;
    while (getline(config, commandline, '\n')) {
        std::vector<std::string> command;
        std::stringstream commandstream(commandline);
        std::string commandword;
        while (commandstream >> commandword) {
            command.push_back(commandword);
        }
        if (command.at(0).at(0) == '#') {                                            // comments
        } else if (command.at(0).substr(0, sizeof("compressor")) == "compressor") {  // parse compressor settings
            parseCompressor(command);
        } else {  // unknown command
            UTILS_DIE_IF(!command.at(0).empty(), "unknown command");
        }
    }
}

void Compressor::compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID = 0) {
    UTILS_DIE_IF(compressorID >= compressorParameters.size(), "not that much compressors defined");
    selectedCompressorID = compressorID;

    std::stringstream intermediateIn;
    std::stringstream intermediateOut;

    auto& comp = compressorParameters.at(compressorID);
    for (auto& compressor : compressorParameters)
        if (compressor.getCompressorID() == compressorID) comp = compressor;

    for (uint8_t i = 0; i < comp.getNumberOfCompressorSteps(); ++i) {
        if (i == 0)
            intermediateIn << input.rdbuf();
        else {
            intermediateIn.str("");
            intermediateIn.clear();
            intermediateIn << intermediateOut.rdbuf();
            intermediateOut.str("");
            intermediateOut.clear();
        }

        switch (comp.getAlgorithmIDs().at(i)) {
            case genie::core::AlgoID::BSC: {
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

#include <type_traits>
class Parameter {
 public:
    bool read(std::string parameterIn, uint8_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<uint8_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, bool& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<bool>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, genie::core::AlgoID& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = genie::core::stringToAlgoID(parameterIn);
        return lastParameter;
    };
    bool read(std::string parameterIn, uint16_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<uint16_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, uint32_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<uint32_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, uint64_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        std::istringstream iss(parameterIn);
        iss >> parameterOut;
        return lastParameter;
    };
    bool read(std::string parameterIn, int8_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<int8_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, int16_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<int16_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, int32_t& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = static_cast<int32_t>(std::stoi(parameterIn));
        return lastParameter;
    };
    bool read(std::string parameterIn, std::string& parameterOut) {
        if (parameterIn.empty()) return true;
        bool lastParameter = stripAndIfLast(parameterIn);
        parameterOut = parameterIn;
        return lastParameter;
    };

 private:
    bool stripAndIfLast(std::string& parameter) {
        if (parameter.at(0) == '{') parameter.erase(0, 1);
        if (parameter.back() == '}') {
            parameter.pop_back();
            return true;
        }
        return false;
    }
};

genie::entropy::lzma::LZMAParameters Compressor::readLzmaParameters(std::vector<std::string>& stringpars) {
    genie::entropy::lzma::LZMAParameters pars;
    uint8_t index = 0;
    if (stringpars.empty()) return pars;
    Parameter parameter;
    if (parameter.read(stringpars.at(index++), pars.level) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.dictSize) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.lc) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.lp) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.pb) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.fb) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.numThreads)) {
    }
    if (stringpars.size() != index)
        stringpars.erase(stringpars.begin(), stringpars.begin() + index - 1);
    else
        stringpars.clear();
    return pars;
}

genie::entropy::bsc::BSCParameters Compressor::readBscParameters(std::vector<std::string>& stringpars) {
    genie::entropy::bsc::BSCParameters pars;
    uint8_t index = 0;
    if (stringpars.empty()) return pars;
    Parameter parameter;
    if (parameter.read(stringpars.at(index++), pars.lzpHashSize) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.lzpMinLen) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.blockSorter) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.coder) || index == stringpars.size() ||
        parameter.read(stringpars.at(index++), pars.features)) {
    }
    stringpars.erase(stringpars.begin(), stringpars.begin() + index - 1);
    return pars;
}

genie::entropy::zstd::ZSTDParameters Compressor::readZstdParameters(std::vector<std::string>& stringpars) {
    genie::entropy::zstd::ZSTDParameters pars;
    uint8_t index = 0;
    if (stringpars.empty()) return pars;
    Parameter parameter;
    if (parameter.read(stringpars.at(index++), pars.use_dictionary_flag) ||
        parameter.read(stringpars.at(index++), pars.dictionary_size) ||
        parameter.read(stringpars.at(index++), pars.dictionary)) {
    }
    stringpars.erase(stringpars.begin(), stringpars.begin() + index - 1);
    return pars;
}
void Compressor::parseCompressor(std::vector<std::string> commandline) {
    // compressorID stepID algorithmID {parameter1 parameter2 ... parametern} {invar1 invar2 ... invarn} {outvar1
    // outvar2
    // ... outvarn}

    Parameter parameter;
    uint8_t compressorID = 0;
    parameter.read(commandline.at(1), compressorID);

    genie::core::record::annotation_parameter_set::compressorStep step;
    parameter.read(commandline.at(2), step.stepID);
    parameter.read(commandline.at(3), step.algorithmID);

    size_t index = 4;
    std::vector<std::string> sub(commandline.begin() + index, commandline.end());
    switch (step.algorithmID) {
        case genie::core::AlgoID::LZMA: {
            genie::entropy::lzma::LZMAParameters parameters = readLzmaParameters(sub);
            step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
            step.algorithm_parameters = parameters.convertToAlgorithmParameters();
            break;
        }
        case genie::core::AlgoID::ZSTD: {
            genie::entropy::zstd::ZSTDParameters parameters = readZstdParameters(sub);
            step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
            step.algorithm_parameters = parameters.convertToAlgorithmParameters();
            break;
        }
        case genie::core::AlgoID::BSC: {
            genie::entropy::bsc::BSCParameters parameters = readBscParameters(sub);
            step.useDefaultAlgorithmParameters = parameters.parsAreDefault();
            step.algorithm_parameters = parameters.convertToAlgorithmParameters();
            break;
        }
        default:
            break;
    }
    bool added = false;
    for (auto& compPar : compressorParameters) {
        if (compPar.getCompressorID() == compressorID) {
            compPar.addCompressorStep(step);
            added = true;
        }
    }
    if (!added) {
        compressorParameters.emplace_back(compressorID);
        compressorParameters.back().addCompressorStep(step);
    }

    // genie::core::AlgoID algorithmID = genie::core::AlgoID.value
}

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

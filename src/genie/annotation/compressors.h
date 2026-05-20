/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_COMPRESSORS_H_
#define SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

class CompressorParser;

// ---------------------------------------------------------------------------------------------------------------------
class Compressor {
 public:
    Compressor();
    void parseConfig(std::stringstream& config);
    uint8_t getNrOfCompressorIDs() const { return static_cast<uint8_t>(compressorParameters.size()); }

    void compress(std::stringstream& input, std::stringstream& output, uint8_t compressorID);

    const std::vector<core::record::annotation_parameter_set::CompressorParameterSet>& getCompressorParameters()
        const {
        return compressorParameters;
    }

    void compress(genie::core::record::annotation_access_unit::TypedData& input,
                  std::vector<genie::core::record::annotation_access_unit::TypedData>& output, uint8_t compressorID);

    const core::record::annotation_parameter_set::CompressorParameterSet* getCompressorParameterSet(
        uint8_t compressorID) const {
      for (const auto& params : compressorParameters) {
        if (params.getCompressorID() == compressorID) {
          return &params;
        }
      }
      return nullptr;
    }

    friend class CompressorParser;

 private:
    uint8_t selectedCompressorID;
    std::vector<core::record::annotation_parameter_set::CompressorParameterSet> compressorParameters;

    void parseCompressor(std::vector<std::string> commandline);

    entropy::lzma::LZMAParameters readLzmaParameters(std::vector<std::string>& stringpars);
    entropy::bsc::BSCParameters readBscParameters(std::vector<std::string>& stringpars);
    entropy::zstd::ZSTDParameters readZstdParameters(std::vector<std::string>& stringpars);
};

// ---------------------------------------------------------------------------------------------------------------------

class CompressorParser {
 public:
  struct Parameter {
    static void read(const std::string& parameterIn, uint8_t& parameterOut);
    static void read(const std::string& parameterIn, bool& parameterOut);
    static void read(const std::string& parameterIn, core::AlgoID& parameterOut);
    static void read(const std::string& parameterIn, uint16_t& parameterOut);
    static void read(const std::string& parameterIn, uint32_t& parameterOut);
    static void read(const std::string& parameterIn, uint64_t& parameterOut);
    static void read(const std::string& parameterIn, int8_t& parameterOut);
    static void read(const std::string& parameterIn, int16_t& parameterOut);
    static void read(const std::string& parameterIn, int32_t& parameterOut);
    static void read(const std::string& parameterIn, std::string& parameterOut);
  };

  static std::vector<std::string> tokenizeCurlyHashNumber(const std::string& input);
  static std::vector<std::string> getAlgoParamsGroup(const std::vector<std::string>& tokens,
                                                     size_t& idx);

  static void parseInVarGroup(const std::vector<std::string>& tokens, size_t& idx,
                              std::vector<uint8_t>& in_var_ID, std::vector<uint8_t>& prev_step_ID,
                              std::vector<uint8_t>& prev_out_var_ID);

  static void parseCompletedOutVarGroup(const std::vector<std::string>& tokens, size_t& idx,
                                        std::vector<uint8_t>& completed_out_var_ID);

  static void parseConfig(Compressor& compressor, std::stringstream& config);
  static void parseCompressor(Compressor& compressor, std::vector<std::string> commandline);
};

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_COMPRESSORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

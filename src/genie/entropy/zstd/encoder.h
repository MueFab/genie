/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_
#define SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif

#include <sstream>

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/util/make_unique.h"
#include "genie/util/watch.h"

#include "apps/genie/annotation/code.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace zstd {

class ZSTDParameters {
 public:
    bool use_dictionary_flag;
    uint16_t dictionary_size;
    std::string dictionary;
    ZSTDParameters() : use_dictionary_flag(false), dictionary_size(0), dictionary{} {}
    ZSTDParameters(uint8_t _use_dictionary_flag, uint8_t _dictionary_size, std::string _dictionary)
        : use_dictionary_flag(_use_dictionary_flag), dictionary_size(_dictionary_size), dictionary(_dictionary) {}

    genie::core::record::annotation_parameter_set::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;

    bool parsAreDefault() const { return use_dictionary_flag == false && dictionary_size == 0 && dictionary.empty(); }
};

class ZSTDEncoder {
 public:
    ZSTDEncoder();

    void encode(std::stringstream &input, std::stringstream &output);

    void decode(std::stringstream &input, std::stringstream &output);

    void configure(const ZSTDParameters zstdParameters) {
        use_dictionary_flag = zstdParameters.use_dictionary_flag;
        dictionary_size = zstdParameters.dictionary_size;
        dictionary = zstdParameters.dictionary;
    }

 private:
    uint8_t use_dictionary_flag;
    uint16_t dictionary_size;
    std::string dictionary;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace zstd
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

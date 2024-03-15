/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_JBIG_ENCODER_H
#define GENIE_ENTROPY_JBIG_ENCODER_H

#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif
#include <sstream>
#include <codecs/include/mpegg-codecs.h>
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace jbig {
// ---------------------------------------------------------------------------------------------------------------------
class JBIGparameters {
 public:
    int32_t num_lines_per_stripe;
    bool deterministic_pred;
    bool typical_pred;
    bool diff_layer_typical_pred;
    bool two_line_template;

    JBIGparameters()
        : num_lines_per_stripe(-1),
          deterministic_pred(true),
          typical_pred(false),
          diff_layer_typical_pred(false),
          two_line_template(false) {}

    JBIGparameters(int32_t _num_lines_per_stripe, bool _deterministic_pred, bool _typical_pred,
                   bool _diff_layer_typical_pred, bool _two_line_template)
        : num_lines_per_stripe(_num_lines_per_stripe),
          deterministic_pred(_deterministic_pred),
          typical_pred(_typical_pred),
          diff_layer_typical_pred(_diff_layer_typical_pred),
          two_line_template(_two_line_template) {}


    genie::core::record::annotation_parameter_set::AlgorithmParameters convertToAlgorithmParameters() const;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet(
        uint8_t compressor_ID) const;
};
// ---------------------------------------------------------------------------------------------------------------------

class JBIGEncoder {
 public:
    JBIGEncoder();

    void configure(const JBIGparameters jbigParameters) {
        num_lines_per_stripe = jbigParameters.num_lines_per_stripe;
        deterministic_pred = jbigParameters.deterministic_pred;
        typical_pred = jbigParameters.typical_pred;
        diff_layer_typical_pred = jbigParameters.diff_layer_typical_pred;
        two_line_template = jbigParameters.two_line_template;
    }

    void encode(std::stringstream& input, std::stringstream& output, uint32_t ncols,
        uint32_t nrows);
    void encode(std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t ncols, uint32_t nrows);

    void decode(std::stringstream& input, std::stringstream& output, uint32_t& nrows, uint32_t& ncols);


 private:
    int32_t num_lines_per_stripe;
    bool deterministic_pred;
    bool typical_pred;
    bool diff_layer_typical_pred;
    bool two_line_template;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace jbig
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_JBIG_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

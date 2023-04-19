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

#include "LikelihoodParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

LikelihoodParameters::LikelihoodParameters() : num_gl_per_sample(0), transform_flag(false), dtype_id(0) {}

LikelihoodParameters::LikelihoodParameters(uint8_t num_gl_per_sample, bool transform_flag, uint8_t dtype_id)
    : num_gl_per_sample(num_gl_per_sample), transform_flag(transform_flag), dtype_id(dtype_id) {}

LikelihoodParameters::LikelihoodParameters(util::BitReader& reader) { read(reader); }

void LikelihoodParameters::read(util::BitReader& reader) {
    num_gl_per_sample = static_cast<uint8_t>(reader.read_b(8));
    transform_flag = static_cast<bool>(reader.read_b(1));
    if (transform_flag) dtype_id = static_cast<uint8_t>(reader.read_b(8));
}

void LikelihoodParameters::write(std::ostream& outputfile) const {
    outputfile << std::to_string(num_gl_per_sample) << ",";
    outputfile << std::to_string(transform_flag) << ",";
    outputfile << std::to_string(dtype_id);
}

void LikelihoodParameters::write(util::BitWriter& writer) const {
    writer.write(num_gl_per_sample,8);
    writer.write(transform_flag,1);
    if (transform_flag) writer.write(dtype_id,8);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

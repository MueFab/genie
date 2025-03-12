/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters()
    : num_gl_per_sample(0), transform_flag(false), dtype_id(core::DataType::STRING) {}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, core::DataType _dtype_id)
    : num_gl_per_sample(_num_gl_per_sample), transform_flag(_transform_flag), dtype_id(_dtype_id) {}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LikelihoodParameters::getNumGlPerSample() const { return num_gl_per_sample; }

// ---------------------------------------------------------------------------------------------------------------------

bool LikelihoodParameters::getTransformFlag() const { return transform_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::DataType LikelihoodParameters::getDtypeID() const { return dtype_id; }

// ---------------------------------------------------------------------------------------------------------------------
void LikelihoodParameters::read(util::BitReader& reader) {
    num_gl_per_sample = static_cast<uint8_t>(reader.ReadBits(8));
    transform_flag = static_cast<bool>(reader.ReadBits(1));
    if (transform_flag) dtype_id = static_cast<genie::core::DataType>(reader.ReadBits(8));
}
// ---------------------------------------------------------------------------------------------------------------------

void LikelihoodParameters::write(genie::core::Writer& writer) const {
  writer.Write(num_gl_per_sample, 8);
    writer.Write(transform_flag, 1);
    if (transform_flag) writer.Write(static_cast<uint8_t>(dtype_id), 8);
}

// ---------------------------------------------------------------------------------------------------------------------

void LikelihoodParameters::write(genie::util::BitWriter& writer) const {
  writer.WriteBits(num_gl_per_sample, 8);
  writer.WriteBits(transform_flag, 1);
  if (transform_flag) writer.WriteBits(static_cast<uint8_t>(dtype_id), 8);
}
// ---------------------------------------------------------------------------------------------------------------------

size_t LikelihoodParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.GetBitsWritten();
}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
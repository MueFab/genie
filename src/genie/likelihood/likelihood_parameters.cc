/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::likelihood {

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters()
    : num_gl_per_sample(0), transform_flag(false), dtype_id(core::DataType::STRING) {}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters(uint8_t _num_gl_per_sample, bool _transform_flag, core::DataType _dtype_id)
    : num_gl_per_sample(_num_gl_per_sample), transform_flag(_transform_flag), dtype_id(_dtype_id) {}

// ---------------------------------------------------------------------------------------------------------------------

LikelihoodParameters::LikelihoodParameters(
    util::BitReader& reader)
    : num_gl_per_sample(reader.ReadAlignedInt<uint8_t>()),
      transform_flag(static_cast<bool>(reader.ReadAlignedInt<uint8_t>() & 0x01)) {
  if (transform_flag) {
    dtype_id = static_cast<genie::core::DataType>(reader.ReadAlignedInt<uint8_t>());
  }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t LikelihoodParameters::GetNumGlPerSample() const { return num_gl_per_sample; }

// ---------------------------------------------------------------------------------------------------------------------

bool LikelihoodParameters::GetTransformFlag() const { return transform_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::DataType LikelihoodParameters::GetDtypeId() const { return dtype_id; }

// ---------------------------------------------------------------------------------------------------------------------

void LikelihoodParameters::Read(util::BitReader& reader) {
    num_gl_per_sample = static_cast<uint8_t>(reader.ReadBits(8));
    reader.ReadBits(7);  // reserved
    transform_flag = static_cast<bool>(reader.ReadBits(1));
    if (transform_flag) dtype_id = static_cast<genie::core::DataType>(reader.ReadBits(8));
}

// ---------------------------------------------------------------------------------------------------------------------

size_t LikelihoodParameters::GetSize() const {
  size_t size = 0;

  size += sizeof(num_gl_per_sample);
  size += sizeof(uint8_t);

  if (transform_flag) {
    size += sizeof(uint8_t);
  }

  return size;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t LikelihoodParameters::GetSize(util::BitWriter& writesize) const {
    Write(writesize);
    return writesize.GetTotalBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

void LikelihoodParameters::Write(util::BitWriter& writer) const {
    writer.WriteBits(num_gl_per_sample, 8);
    writer.WriteReserved(7);
    writer.WriteBits(transform_flag, 1);
    if (transform_flag) writer.WriteBits(static_cast<uint64_t>(dtype_id), 8);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::likelihood

// ---------------------------------------------------------------------------------------------------------------------

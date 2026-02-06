/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_TYPES_H
#define GENIE_GENOTYPE_TYPES_H

#include <cstdint>
#include <vector>

namespace genie::genotype {

// Pure C++ types for records, parameter sets and blocks
using BinMatDtype = std::vector<std::vector<bool>>;
using UInt8MatDtype = std::vector<std::vector<uint8_t>>;
using Int8MatDtype = std::vector<std::vector<int8_t>>;
using UIntVecDtype = std::vector<uint32_t>;
using MatShapeDtype = std::vector<size_t>;

} // namespace genie::genotype

#endif // GENIE_GENOTYPE_TYPES_H

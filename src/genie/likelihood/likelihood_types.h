/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_TYPES_H
#define GENIE_LIKELIHOOD_TYPES_H

#include <cstdint>
#include <vector>

namespace genie::likelihood {

// Pure C++ types for records, parameter sets and blocks
using UInt32ArrDtype = std::vector<uint32_t>;
using UInt32MatDtype = std::vector<std::vector<uint32_t>>;
using MatShapeDtype = std::vector<size_t>;

} // namespace genie::likelihood

#endif // GENIE_LIKELIHOOD_TYPES_H
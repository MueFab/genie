/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_TYPES_H
#define GENIE_CONTACT_TYPES_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace genie::contact {

// Pure C++ types for records, parameter sets and blocks
using BinVecDtype = std::vector<bool>;
using BinMatDtype = std::vector<std::vector<bool>>;
using UInt8VecDtype = std::vector<uint8_t>;
using UIntVecDtype = std::vector<uint32_t>;
using UIntMatDtype = std::vector<std::vector<uint32_t>>;
using UInt64VecDtype = std::vector<uint64_t>;
using Int64VecDtype = std::vector<int64_t>;
using DoubleVecDtype = std::vector<double>;
using MatShapeDtype = std::vector<size_t>;

enum class TransformID : uint8_t {
    ID_0 = 0,
    ID_1 = 1,
    ID_2 = 2,
    ID_3 = 3,
};

struct RunLengthEncodingData {
  bool firstVal;
  UIntVecDtype rl_entries;
  uint32_t maxCount;
  TransformID transformID;
};

} // namespace genie::contact

#endif // GENIE_CONTACT_TYPES_H
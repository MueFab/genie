/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ARRAYTYPE_H_
#define SRC_GENIE_CORE_ARRAYTYPE_H_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace core {

enum class Type {
    STRING = 0,
    CHAR,
    BOOL,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    FLOAT,
    DOUBLE
};


class arrayType {
 private:
    uint8_t bitSize;

 public:
    std::vector<uint8_t> toArray(uint8_t type, util::BitReader& reader);
    void toFile(uint8_t type, std::vector<uint8_t> bytearray, core::Writer& writer) const;
    std::string toString(uint8_t type, std::vector<uint8_t> value) const;

    uint8_t getDefaultBitsize(uint8_t type) const;
    uint64_t getDefaultValue(uint8_t type) const;
    std::vector<uint8_t> toArray(uint8_t type, uint64_t value);
};

}  // namespace core
}  // namespace genie
#endif  // SRC_GENIE_CORE_ARRAYTYPE_H_
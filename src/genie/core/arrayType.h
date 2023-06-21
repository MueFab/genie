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
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/writer.h"

namespace genie {
namespace core {

class arrayType {
 private:
    uint8_t bitSize;
    public:

    std::vector<uint8_t> toArray(uint8_t type, util::BitReader& reader);
    void toFile(uint8_t type, std::vector<uint8_t> bytearray, core::Writer& writer) const;
    std::string toString(uint8_t type, std::vector<uint8_t> value) const;

    uint8_t getDefaultBitsize(uint8_t type);
    uint64_t getDefaultValue(uint8_t type);
};

}  // namespace core
}  // namespace genie
#endif  // SRC_GENIE_CORE_ARRAYTYPE_H_
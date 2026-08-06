/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ARRAY_TYPE_H_
#define SRC_GENIE_CORE_ARRAY_TYPE_H_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

namespace genie::core {

class ArrayType {
 private:
    uint8_t bitSize = 0;

 public:
    std::vector<uint8_t> toArray(DataType type, util::BitReader& reader) const;

    void toFile(core::DataType type, std::vector<uint8_t> bytearray, util::BitWriter& writer) const;
    void toFile(core::DataType type, util::BitReader& reader, util::BitWriter& writer, uint64_t number) const;

    std::string toString(core::DataType type, std::vector<uint8_t> value) const;

    uint8_t getDefaultBitsize(core::DataType type) const;
    uint64_t getDefaultValue(core::DataType type) const;
    std::vector<uint8_t> toArray(core::DataType type, uint64_t value) const;
};

}  // namespace genie::core
#endif  // SRC_GENIE_CORE_ARRAY_TYPE_H_

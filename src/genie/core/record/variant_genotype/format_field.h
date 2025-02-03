/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H
#define GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H

#include <boost/variant/variant.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

class FormatField {
 private:
    std::string format_name;
    DataType format_type;
    // sample_count, array_len
    std::vector<std::vector<DynamicDataType>> format_value;

 public:
    uint8_t getFormatLen() const;
    const std::string& getFormatName() const;
    DataType getFormatType() const;
    uint32_t getSampleCount() const;
    uint8_t getFormatArrayLen() const;
    const std::vector<std::vector<DynamicDataType>>& getFormatValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H

// ---------------------------------------------------------------------------------------------------------------------

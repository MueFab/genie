/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream_protection.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamProtection::DescriptorStreamProtection(std::string _dSProtectionValue)
    : DSProtectionValue(std::move(_dSProtectionValue)) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamProtection::DescriptorStreamProtection(genie::util::BitReader& reader) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    DSProtectionValue.resize(length);
    reader.readAlignedBytes(DSProtectionValue.data(), DSProtectionValue.length());
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStreamProtection::getProtectionValue() const { return DSProtectionValue; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStreamProtection::getKey() const {
    static const std::string key = "dspr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStreamProtection::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeAlignedBytes(DSProtectionValue.data(), DSProtectionValue.size());
}

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStreamProtection::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DescriptorStreamProtection&>(info);
    return DSProtectionValue == other.DSProtectionValue;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string DescriptorStreamProtection::decapsulate() { return std::move(DSProtectionValue); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

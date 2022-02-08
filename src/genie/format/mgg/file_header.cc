/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/file_header.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMajorBrand() const { return major_brand; }

// ---------------------------------------------------------------------------------------------------------------------

core::MPEGMinorVersion FileHeader::getMinorVersion() const { return minor_version; }

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::addCompatibleBrand(std::string brand) { compatible_brands.emplace_back(std::move(brand)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::getCompatibleBrands() const { return compatible_brands; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getKey() const {
    static const std::string key = "flhd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(core::MPEGMinorVersion _minor_version) : major_brand("MPEG-G"), minor_version(_minor_version) {}

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(genie::util::BitReader& bitreader) : major_brand(6, '\0'), minor_version() {
    auto start_pos = bitreader.getPos() - 4;
    auto length = bitreader.readBypassBE<uint64_t>();
    auto num_compatible_brands = (length - 22) / 4;
    bitreader.readBypass(major_brand);
    UTILS_DIE_IF(major_brand != "MPEG-G", "Not an MPEG-G file");
    std::string tmp(4, '\0');
    bitreader.readBypass(tmp);
    minor_version = core::getMPEGVersion(tmp);
    compatible_brands.resize(num_compatible_brands, std::string(4, '\0'));
    for (auto& b : compatible_brands) {
        bitreader.readBypass(b);
    }
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeBypass(major_brand.data(), major_brand.length());
    auto tmp = core::getMPEGVersionString(minor_version);
    bitWriter.writeBypass(tmp.data(), tmp.length());
    for (auto& b : compatible_brands) {
        bitWriter.writeBypass(b.data(), b.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool FileHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const FileHeader&>(info);
    return major_brand == other.major_brand && minor_version == other.minor_version &&
           compatible_brands == other.compatible_brands;
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* File Header");
    print_offset(output, depth + 1, max_depth, "Major brand: " + major_brand);
    print_offset(output, depth + 1, max_depth, "Minor version: " + core::getMPEGVersionString(minor_version));
    for (const auto& b : compatible_brands) {
        print_offset(output, depth + 1, max_depth, "Compatible brand: " + b);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

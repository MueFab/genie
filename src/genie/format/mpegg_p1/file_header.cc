/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/file_header.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMajorBrand() const { return major_brand; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMinorVersion() const { return minor_version; }

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::addCompatibleBrand(std::string brand) { compatible_brands.emplace_back(std::move(brand)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::getCompatibleBrands() const { return compatible_brands; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getKey() const {
    static const std::string key = "fldh";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(std::string _minor_version) : major_brand("MPEG-G"), minor_version(std::move(_minor_version)) {}

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(genie::util::BitReader& bitreader) : major_brand(6, '\0'), minor_version(4, '\0') {
    auto length = bitreader.readBypassBE<uint64_t>();
    auto num_compatible_brands = (length - 22) / 4;
    bitreader.readBypass(major_brand);
    UTILS_DIE_IF(major_brand != "MPEG-G", "Not an MPEG-G file");
    bitreader.readBypass(minor_version);
    UTILS_DIE_IF(minor_version != "2000", "Unsupported version of MPEG-G");
    compatible_brands.resize(num_compatible_brands, std::string(4, '\0'));
    for (auto& b : compatible_brands) {
        bitreader.readBypass(b);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    bitWriter.writeBypass(major_brand.data(), major_brand.length());
    bitWriter.writeBypass(minor_version.data(), minor_version.length());
    for (auto& b : compatible_brands) {
        bitWriter.writeBypass(b.data(), b.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t FileHeader::getSize() const {
    return GenInfo::getSize() + major_brand.length() + minor_version.length() + compatible_brands.size() * 4;
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

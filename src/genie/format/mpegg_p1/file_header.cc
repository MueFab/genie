/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/file_header.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(std::vector<std::string>* const x_compatible_brand) {
    compatible_brand = std::move(*x_compatible_brand);
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("flhd");

    uint64_t length = 12 + 10 + compatible_brand.size() * 4;
    bitWriter.write(length, 64);

    bitWriter.write(major_brand);
    bitWriter.write(minor_brand);
    for (auto const& it : compatible_brand) {
        bitWriter.write(it);
    }
    bitWriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::addCompatibleBrand(const std::string& brand) { compatible_brand.push_back(brand); }

// ---------------------------------------------------------------------------------------------------------------------

const char* FileHeader::getMajorBrand() const { return major_brand; }

// ---------------------------------------------------------------------------------------------------------------------

const char* FileHeader::getMinorBrand() const { return minor_brand; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::getCompatibleBrand() const { return compatible_brand; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

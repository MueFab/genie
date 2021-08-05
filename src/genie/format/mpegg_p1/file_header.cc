/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/file_header.h"
#include <utility>
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(std::vector<std::string>* const x_compatible_brand) {
    compatible_brand = std::move(*x_compatible_brand);
}

// ---------------------------------------------------------------------------------------------------------------------

FileHeader::FileHeader(genie::util::BitReader& bitreader):
    major_brand("MPEG-G"), minor_version("2000"),
    compatible_brand(){

    size_t start_pos = bitreader.getPos();

    std::string key = genie::format::mpegg_p1::readKey(bitreader);
    UTILS_DIE_IF(key !=  "flhd", "File header not found");

    uint64_t length = bitreader.read<uint64_t>();
    uint32_t num_compatible_brands = (length-22)/4;

    std::string f_major_brand(readFixedLengthChars(bitreader, 6));
    UTILS_DIE_IF(f_major_brand != major_brand, "Invalid major_brand in file header");

    minor_version = readFixedLengthChars(bitreader, 4);

    for (uint32_t i = 0; i<num_compatible_brands; i++){
        compatible_brand.emplace_back(readFixedLengthChars(bitreader, 4));
    }

    size_t true_length = bitreader.getPos() - start_pos;
    UTILS_DIE_IF(!bitreader.isAligned() || true_length != length, "Invalid length of KLV");
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("flhd");

    uint64_t length = 12 + 10 + compatible_brand.size() * 4;
    bitWriter.write(length, 64);

    bitWriter.write(major_brand);
    bitWriter.write(minor_version);
    for (auto const& it : compatible_brand) {
        bitWriter.write(it);
    }
    bitWriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void FileHeader::addCompatibleBrand(const std::string& brand) { compatible_brand.push_back(brand); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMajorBrand() const { return major_brand; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMinorVersion() const { return minor_version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::getCompatibleBrand() const { return compatible_brand; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

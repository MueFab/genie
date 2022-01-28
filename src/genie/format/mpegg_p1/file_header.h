/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/make-unique.h"
#include "genie/format/mpegg_p1/gen_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class FileHeader : public GenInfo {
 public:

    const std::string& getKey() const override {
        static const std::string key = "fldh";
        return key;
    }

    /**
     * @brief
     */
    FileHeader() = default;

    /**
     * @brief
     */
    explicit FileHeader(std::string _minor_version) : major_brand("MPEG-G"), minor_version(std::move(_minor_version)) {}

    /**
     * @brief
     */
    explicit FileHeader(genie::util::BitReader& bitreader) : major_brand(6, '\0') , minor_version(4, '\0'){
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

    /**
     * @brief
     * @param brand
     */
    void addCompatibleBrand(std::string brand) {
        compatible_brands.emplace_back(std::move(brand));
    }

    /**
     * @brief
     * @return
     */
    const std::string& getMajorBrand() const;

    /**
     * @brief
     * @return
     */
    const std::string& getMinorVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getCompatibleBrands() const;

    /**
     * @brief
     * @param bitWriter
     */
    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.writeBypass(major_brand.data(), major_brand.length());
        bitWriter.writeBypass(minor_version.data(), minor_version.length());
        for (auto& b : compatible_brands) {
            bitWriter.writeBypass(b.data(), b.length());
        }
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + major_brand.length() + minor_version.length() + compatible_brands.size() * 4;
    }

 private:

    std::string major_brand;
    std::string minor_version;
    std::vector<std::string> compatible_brands;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

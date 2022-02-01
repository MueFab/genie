/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class FileHeader : public GenInfo {
 public:
    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     */
    FileHeader() = default;

    /**
     * @brief
     */
    explicit FileHeader(core::MPEGMinorVersion _minor_version);

    /**
     * @brief
     */
    explicit FileHeader(genie::util::BitReader& bitreader);

    /**
     * @brief
     * @param brand
     */
    void addCompatibleBrand(std::string brand);

    /**
     * @brief
     * @return
     */
    const std::string& getMajorBrand() const;

    /**
     * @brief
     * @return
     */
    core::MPEGMinorVersion getMinorVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getCompatibleBrands() const;

    /**
     * @brief
     * @param bitWriter
     */
    void box_write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* File Header");
        print_offset(output, depth + 1, max_depth, "Major brand: " + major_brand);
        print_offset(output, depth + 1, max_depth, "Minor version: " + core::getMPEGVersionString(minor_version));
        for (const auto& b : compatible_brands) {
            print_offset(output, depth + 1, max_depth, "Compatible brand: " + b);
        }
    }

 private:
    std::string major_brand;                     //!< @brief
    core::MPEGMinorVersion minor_version;        //!< @brief
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

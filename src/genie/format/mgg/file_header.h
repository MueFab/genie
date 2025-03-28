/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_
#define SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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
    FileHeader();

    /**
     * @brief
     */
    explicit FileHeader(core::MpegMinorVersion _minor_version);

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
    core::MpegMinorVersion getMinorVersion() const;

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

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

 private:
    std::string major_brand;                     //!< @brief
    core::MpegMinorVersion minor_version;        //!< @brief
    std::vector<std::string> compatible_brands;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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
    explicit FileHeader(std::string _minor_version);

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
    void write(genie::util::BitWriter& bitWriter) const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

 private:
    std::string major_brand;                     //!< @brief
    std::string minor_version;                   //!< @brief
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

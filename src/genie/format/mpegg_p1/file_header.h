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
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class FileHeader {
 public:
    /**
     * @brief
     */
    FileHeader() = default;

    /**
     * @brief
     */
    explicit FileHeader(std::vector<std::string>*);  // FIXME:strings needs to be checked for size!

    /**
     * @brief
     * @param brand
     */
    void addCompatibleBrand(const std::string& brand);

    /**
     * @brief
     * @return
     */
    const char* getMajorBrand() const;

    /**
     * @brief
     * @return
     */
    const char* getMinorBrand() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::string>& getCompatibleBrand() const;

    /**
     * @brief
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

 private:
    const char* const major_brand = "MPEG-G";   //!< @brief
    const char* const minor_brand = "2000";     //!< @brief @FIXME: 19 or 20?
    std::vector<std::string> compatible_brand;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_FILE_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

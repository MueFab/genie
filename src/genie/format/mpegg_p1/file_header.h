/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FILE_HEADER_H
#define GENIE_FILE_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {
/**
 *
 */
class FileHeader {
   public:
    /**
     *
     */
    explicit FileHeader() = default;
    /**
     *
     */
    explicit FileHeader(std::vector<std::string>*);  // FIXME:strings needs to be checked for size!

    /**
     *
     * @param brand
     */
    void addCompatibleBrand(const std::string& brand);

    /**
     *
     * @return
     */
    const char* getMajorBrand() const;

    /**
     *
     * @return
     */
    const char* getMinorBrand() const;

    /**
     *
     * @return
     */
    const std::vector<std::string>& getCompatibleBrand() const;

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    const char* const major_brand = "MPEG-G";   //!<
    const char* const minor_brand = "2000";     //!< FIXME: 19 or 20?
    std::vector<std::string> compatible_brand;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FILE_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
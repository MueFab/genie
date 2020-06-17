/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FILE_HEADER_H
#define GENIE_FILE_HEADER_H

#include <string>
#include <vector>
#include "genie/util/bitwriter.h"

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
    void addCompatibleBrand(const std::string& brand) {
        compatible_brand.push_back(brand);
    }  // FIXME:string needs to be checked for size!

    /**
     *
     * @return
     */
    const char* getMajorBrand() const { return major_brand; }

    /**
     *
     * @return
     */
    const char* getMinorBrand() const { return minor_brand; }

    /**
     *
     * @return
     */
    const std::vector<std::string>& getCompatibleBrand() const { return compatible_brand; }

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    /**
     * ISO 23092-1 Section 6.6.2 table 30
     *
     * ------------------------------------------------------------------------------------------------------------- */
    const char* const major_brand = "MPEG-G";   //!<
    const char* const minor_brand = "2000";     //!< FIXME: 19 or 20?
    std::vector<std::string> compatible_brand;  //!<
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_FILE_HEADER_H

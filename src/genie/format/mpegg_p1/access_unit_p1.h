/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_P1_H
#define GENIE_ACCESS_UNIT_P1_H

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/data-unit-factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class AU_information {
   public:
    /**
     *
     */
    AU_information();

   private:
    std::vector<uint8_t> AU_information_value;  //!<
};

/**
 *
 */
class AU_protection {
   public:
    /**
     *
     */
    AU_protection();

   private:
    std::vector<uint8_t> AU_protection_value;  //!<
};

/**
 *
 */
class AccessUnit {
   public:
    /**
     *
     * @param au_p2
     */
    explicit AccessUnit(genie::format::mgb::AccessUnit&& au_p2);

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:

    genie::format::mgb::AccessUnit au_p1;  //!< if MIT_flag is set to 0 -> au_p1 header equates to au_p2 header

    // std::unique_ptr<AU_information> au_information;   //optional
    // std::unique_ptr<AU_protection> au_protection;     //optional
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_P1_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
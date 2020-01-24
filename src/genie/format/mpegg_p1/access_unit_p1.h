/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_P1_H
#define GENIE_ACCESS_UNIT_P1_H

#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/data-unit-factory.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class AU_information {
   public:
    AU_information() : AU_information_value() {
        AU_information_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                                0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                                0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> AU_information_value;
};

class AU_protection {
   public:
    AU_protection() : AU_protection_value() {
        AU_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                               0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                               0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> AU_protection_value;
};

class AccessUnit {
   public:
    explicit AccessUnit(genie::format::mgb::AccessUnit&& au_p2);

   private:
    /**
     * ISO 23092-1 Section 6.5.3 table 24
     *
     * ------------------------------------------------------------------------------------------------------------- */

    genie::format::mgb::AccessUnit au_p1;

    // std::unique_ptr<AU_information> au_information;   //optional
    // std::unique_ptr<AU_protection> au_protection;     //optional
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_ACCESS_UNIT_P1_H

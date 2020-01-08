#ifndef GENIE_ACCESS_UNIT_P1_H
#define GENIE_ACCESS_UNIT_P1_H

#include "format/mpegg_p2/access_unit.h"

namespace format {
namespace mpegg_p1 {

class AU_information {
   public:
    AU_information() : AU_information_value() {
        // TODO: init with void.lzma
        AU_information_value = {};
    }

   private:
    std::vector<uint8_t> AU_information_value;
};

// TODO: rename to format::mpegg_p1::AccessUnit
class AccessUnitP1 : public format::mpegg_p2::AccessUnit {
   private:
    /**
     * ISO 23092-1 Section 6.5.3 table 24
     *
     * ------------------------------------------------------------------------------------------------------------- */

    std::unique_ptr<AU_information> au_information;
    // AU_information    TODO
    // AU_protection     TODO
};

}  // namespace mpegg_p1
}  // namespace format

#endif //GENIE_ACCESS_UNIT_P1_H

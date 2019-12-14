#ifndef GENIE_ACCESS_UNIT_P1_H
#define GENIE_ACCESS_UNIT_P1_H

#include "format/mpegg_p2/access_unit.h"

namespace format {
    namespace mpegg_p1 {
    class AccessUnitP1 : public format::mpegg_p2::AccessUnit{
        /**
        * ISO 23092-1 Section 6.5.3 table 24
        *
        * ------------------------------------------------------------------------------------------------------------- */

            //AU_information    TODO
            //AU_protection     TODO
        };
    }//namespace mpegg_p1
}// namespace format

#endif //GENIE_ACCESS_UNIT_P1_H

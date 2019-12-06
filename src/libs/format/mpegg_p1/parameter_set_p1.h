#ifndef GENIE_PARAMETER_SET_P1_H
#define GENIE_PARAMETER_SET_P1_H

#include "format/mpegg_p2/parameter_set.h"

namespace format {
    namespace mpegg_p1 {
    class ParameterSetP1 : public format::mpegg_p2::ParameterSet {
    private:
        /**
        * ISO 23092-1 Section 6.5.2 table 23
        *
        * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint16_t dataset_ID : 16;
    };

    }//namespace mpegg_p1
}// namespace format

#endif //GENIE_PARAMETER_SET_H

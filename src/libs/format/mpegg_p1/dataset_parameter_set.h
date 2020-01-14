#ifndef GENIE_DATASET_PARAMETER_SET_H
#define GENIE_DATASET_PARAMETER_SET_H

#include "format/mpegg_p2/parameter_set.h"

namespace format {
namespace mpegg_p1 {
class DatasetParameterSet : public format::mpegg_p2::ParameterSet {
   public:
    DatasetParameterSet(format::mpegg_p2::ParameterSet&);

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 23
     *
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint16_t dataset_ID : 16;
};

}  // namespace mpegg_p1
}  // namespace format

#endif  // GENIE_PARAMETER_SET_H

#ifndef GENIE_DATASET_H
#define GENIE_DATASET_H

#include <memory>
#include <vector>

#include "dataset_header.h"
#include "dataset_parameter_set.h"
#include "access_unit_p1.h"

namespace format {
namespace mpegg_p1 {
class Dataset {
   public:
    static std::unique_ptr<format::mpegg_p1::Dataset> createFromDataUnits(
        const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>>& dataUnits);

        private:
            /**
             * ISO 23092-1 Section 6.5.2 table 18
             *
             * ------------------------------------------------------------------------------------------------------------- */
             std::unique_ptr<format::mpegg_p1::DatasetHeader> dataset_header;
             //DT_metadata      TODO
             //DT_protection    TODO
             std::vector<std::unique_ptr<DatasetParameterSet>> dataset_parameter_sets;
             /** master_index_table is optional and not yet implemented */
             std::vector<std::unique_ptr<format::mpegg_p1::AccessUnitP1>> access_units;
             /** descriptor_stream[] is optional and not yet implemented */
        };
    }// namespace mpegg_p1
}// namespace format

#endif //GENIE_DATASET_H

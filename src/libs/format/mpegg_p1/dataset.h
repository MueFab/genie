#ifndef GENIE_DATASET_H
#define GENIE_DATASET_H

#include <memory>
#include <vector>

#include "dataset_header.h"
#include "parameter_set_p1.h"

namespace format {
    namespace mpegg_p1 {
        class Dataset {
        public:

        private:
            /**
             * ISO 23092-1 Section 6.5.2 table 18
             *
             * ------------------------------------------------------------------------------------------------------------- */
             std::unique_ptr<format::mpegg_p1::DatasetHeader> dataset_header;
             std::vector<std::unique_ptr<ParameterSetP1>> dataset_parameter_sets;
             /** master_index_table is optional and not yet implemented */
             //std::vector<std::unique_ptr<format::>> dataset_access_units; //TODO: Part 1 oder Part 2?

        };
    }// namespace mpegg_p1
}// namespace format

#endif //GENIE_DATASET_H

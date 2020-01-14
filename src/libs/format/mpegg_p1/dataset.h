#ifndef GENIE_DATASET_H
#define GENIE_DATASET_H

#include <memory>
#include <vector>

#include "access_unit_p1.h"
#include "dataset_header.h"
#include "dataset_parameter_set.h"
#include "format/mpegg_p2/data_unit.h"

namespace format {
namespace mpegg_p1 {

class DT_metadata {
   public:
    DT_metadata() : DT_metadata_value() {
        DT_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                             0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                             0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DT_metadata_value;
};

class DT_protection {
   public:
    DT_protection() : DT_protection_value() {
        DT_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                               0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                               0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DT_protection_value;
};

class Dataset {
   public:
    explicit Dataset(const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>>& dataUnits);

    static std::unique_ptr<format::mpegg_p1::Dataset> createFromDataUnits(
        const std::vector<std::unique_ptr<format::mpegg_p2::DataUnit>>& dataUnits);

   private:
    /**
     * ISO 23092-1 Section 6.5.2 table 18
     *
     * ------------------------------------------------------------------------------------------------------------- */
    std::unique_ptr<format::mpegg_p1::DatasetHeader> dataset_header;
    std::unique_ptr<DT_metadata> dt_metadata;
    std::unique_ptr<DT_protection> dt_protection;
    std::vector<std::unique_ptr<DatasetParameterSet>> dataset_parameter_sets;
    /** master_index_table is optional and not yet implemented */
    std::vector<std::unique_ptr<format::mpegg_p1::AccessUnit>> access_units;
    /** descriptor_stream[] is optional and not yet implemented */
};
}  // namespace mpegg_p1
}  // namespace format

#endif  // GENIE_DATASET_H

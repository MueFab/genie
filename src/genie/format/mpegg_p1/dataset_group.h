#ifndef GENIE_DATASET_GROUP_H
#define GENIE_DATASET_GROUP_H

#include <memory>
#include <vector>

#include "dataset.h"
#include "dataset_group_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DG_metadata {
   public:
    DG_metadata() : DG_metadata_value() {
        DG_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                             0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                             0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DG_metadata_value;
};

class DG_protection {
   public:
    DG_protection() : DG_protection_value() {
        DG_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                               0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                               0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
    }

   private:
    std::vector<uint8_t> DG_protection_value;
};

class DatasetGroup {
   public:
   private:
    /**
     * ISO 23092-1 Section 6.5.1 table 8
     *
     * ------------------------------------------------------------------------------------------------------------- */
    std::unique_ptr<mpegg_p1::DatasetGroupHeader> dataset_group_header;
    std::unique_ptr<DG_metadata> dg_metadata;      // optional
    std::unique_ptr<DG_protection> dg_protection;  // optional
    /** reference[] and reference_metadata[] and label_list is optional and not yet implemented */
    std::vector<std::unique_ptr<Dataset>> dataset;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_DATASET_GROUP_H

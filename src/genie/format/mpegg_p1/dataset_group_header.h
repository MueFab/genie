#ifndef GENIE_DATASET_GROUP_HEADER_H
#define GENIE_DATASET_GROUP_HEADER_H

#include <cstdint>
#include <vector>

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetGroupHeader {
   public:

   private:
    /**
    * ISO 23092-1 Section 6.5.1.2 table 19
    *
    * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID : 8;
    uint8_t version_number : 8;
    std::vector<uint16_t> dataset_ID;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
#endif  // GENIE_DATASET_GROUP_HEADER_H

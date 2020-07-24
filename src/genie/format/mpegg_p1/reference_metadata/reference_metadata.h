#ifndef GENIE_PART1_REFERENCE_METADATA_H
#define GENIE_PART1_REFERENCE_METADATA_H

#include <cstdint>
#include <vector>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceMetadata {
   private:
    uint8_t dataset_group_ID;
    uint8_t reference_ID;
    std::vector<uint8_t> reference_metadata_value;

   public:
    ReferenceMetadata();

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
#endif  // GENIE_PART1_REFERENCE_METADATA_H

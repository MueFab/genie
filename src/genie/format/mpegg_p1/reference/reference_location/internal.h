#ifndef GENIE_PART1_REFERENCE_LOCATION_INTERNAL_H
#define GENIE_PART1_REFERENCE_LOCATION_INTERNAL_H

#include <string>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "reference_location.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Internal : public ReferenceLocation {
   private:
    uint8_t dataset_group_ID;
    uint16_t dataset_ID;

   public:
//    Internal();
    Internal(uint8_t _dataset_group_ID, uint16_t _dataset_ID);

    explicit Internal(util::BitReader& reader);

    uint64_t getBitLength() const override;

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_LOCATION_INTERNAL_H

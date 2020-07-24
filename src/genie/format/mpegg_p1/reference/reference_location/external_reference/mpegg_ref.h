#ifndef GENIE_PART1_EXTERNAL_REFERENCE_MPEGG_REF_H
#define GENIE_PART1_EXTERNAL_REFERENCE_MPEGG_REF_H

#include <genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h>
#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class MpegReference: public ExternalReference {
   private:
    // external_dataset_group_ID
    uint8_t dataset_group_ID;
    // external_dataset_ID
    uint16_t dataset_ID;

    Checksum ref_checksum;

   public:
    MpegReference(uint8_t _dataset_group_ID, uint16_t _dataset_ID, Checksum &&_ref_checksum);

    void addChecksum(Checksum &&_checksum);

    void write(genie::util::BitWriter& bit_writer) const override;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_MPEGG_REF_H

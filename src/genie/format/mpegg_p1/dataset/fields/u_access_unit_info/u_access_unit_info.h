#ifndef GENIE_PART1_DATASET_HEADER_U_ACCESS_UNIT_INFO_H
#define GENIE_PART1_DATASET_HEADER_U_ACCESS_UNIT_INFO_H

#include <cstdint>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class UAccessUnitInfo {
   private:
    uint32_t num_U_clusters;
    uint32_t multiple_signature_base;
    uint8_t U_signature_size;
    bool U_signature_constant_length;
    uint8_t U_signature_length;
   public:

    UAccessUnitInfo();

    explicit UAccessUnitInfo(uint32_t _num_U_clusters);

    void setMultipleSignature(uint32_t base, uint8_t size);
    void setConstantSignature(uint8_t sign_length);

    /**
     * Get length in bit
     *
     * @return
     */
    uint64_t getBitLength() const;

    /**
     * Write to bit_writer
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_U_ACCESS_UNIT_INFO_H

#ifndef GENIE_PART1_EXTERNAL_REFERENCE_RAW_REF_H
#define GENIE_PART1_EXTERNAL_REFERENCE_RAW_REF_H

#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

#include <genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h>
#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class RawReference: public ExternalReference{
   private:
    std::vector<Checksum> checksums;

   public:
    RawReference();

    RawReference(util::BitReader& reader, Checksum::Algo checksum_alg, uint16_t seq_count);

    explicit RawReference(std::vector<Checksum>&& _checksums);

    Checksum::Algo getChecksumAlg() const override;

//    void addChecksum(Checksum &&_checksum);
//
//    void addChecksums(std::vector<Checksum>& _checksums);

    void write(genie::util::BitWriter& bit_writer) override;

    uint64_t getLength() override {return 0;}
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_MPEGG_REF_H

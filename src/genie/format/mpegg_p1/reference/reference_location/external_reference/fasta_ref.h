#ifndef GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H
#define GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H

#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

#include <genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h>
#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class FastaReference: public ExternalReference{
   private:
    std::vector<Checksum> checksums;

   public:
    FastaReference();

    FastaReference(util::BitReader& reader, Checksum::Algo checksum_alg, uint16_t seq_count);

    explicit FastaReference(std::vector<Checksum>&& _checksums);

    void addChecksum(Checksum &&_checksum);

    void addChecksums(std::vector<Checksum>& _checksums);

    void write(genie::util::BitWriter& bit_writer) override;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H

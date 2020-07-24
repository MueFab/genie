#ifndef GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H
#define GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H

#include <genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h>
#include <vector>
#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class FastaReference: public ExternalReference{
   private:
    std::vector<Checksum> checksums;

   public:
    FastaReference();
    explicit FastaReference(std::vector<Checksum>&& _checksums);

    void addChecksum(Checksum &&_checksum);
    void addChecksums(std::vector<Checksum>& _checksums);

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_EXTERNAL_REFERENCE_FASTA_REF_H

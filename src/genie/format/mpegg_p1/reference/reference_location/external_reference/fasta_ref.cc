#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "fasta_ref.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

FastaReference::FastaReference():
    ExternalReference(ExternalReference::Type::FASTA_REF){}

FastaReference::FastaReference(std::vector<Checksum> &&_checksums)
    : ExternalReference(ExternalReference::Type::FASTA_REF),
      checksums(_checksums) {

    for (auto& checksum : checksums){
        UTILS_DIE_IF(checksums.front().getType() != checksum.getType(),
                     "Different checksum algorithm");
    }
}

void FastaReference::addChecksum(Checksum &&_checksum) {
    UTILS_DIE_IF(!checksums.empty() && checksums.front().getType() != _checksum.getType(),
                 "Different checksum algorithm");
    checksums.push_back(_checksum);
}
void FastaReference::addChecksums(std::vector<Checksum> &_checksums) {
    for (auto& checksum : _checksums){
        UTILS_DIE_IF(_checksums.front().getType() != checksum.getType(),
                     "Different checksum algorithm");
    }
    checksums = std::move(_checksums);
}

void FastaReference::write(util::BitWriter &bit_writer) const {
    for (auto& checksum: checksums){
        checksum.write(bit_writer);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

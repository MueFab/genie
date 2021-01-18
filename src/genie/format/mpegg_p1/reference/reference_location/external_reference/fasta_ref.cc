#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "fasta_ref.h"
#include "checksum.h"
#include "md5.h"
#include "sha256.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference():
    ExternalReference(ExternalReference::Type::FASTA_REF){}

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference(util::BitReader &reader, Checksum::Algo checksum_alg, uint16_t seq_count) {
    switch (checksum_alg) {
        case Checksum::Algo::MD5: {
            for (auto i = 0; i< seq_count; i++){
                checksums.push_back(Md5(reader));
            }
            break;
        }
        case Checksum::Algo::SHA256: {
            for (auto i = 0; i< seq_count; i++){
                checksums.push_back(Sha256(reader));
            }
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference(std::vector<Checksum> &&_checksums)
    : ExternalReference(ExternalReference::Type::FASTA_REF),
      checksums(_checksums) {

    for (auto& checksum : checksums){
        UTILS_DIE_IF(checksums.front().getType() != checksum.getType(),
                     "Different checksum algorithm");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//void FastaReference::addChecksum(Checksum &&_checksum) {
//    UTILS_DIE_IF(!checksums.empty() && checksums.front().getType() != _checksum.getType(),
//                 "Different checksum algorithm");
//    checksums.push_back(_checksum);
//}

// ---------------------------------------------------------------------------------------------------------------------

//void FastaReference::addChecksums(std::vector<Checksum> &_checksums) {
//    for (auto& checksum : _checksums){
//        UTILS_DIE_IF(_checksums.front().getType() != checksum.getType(),
//                     "Different checksum algorithm");
//    }
//    checksums = std::move(_checksums);
//}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo FastaReference::getChecksumAlg() const { return checksums.front().getType(); }

// ---------------------------------------------------------------------------------------------------------------------

void FastaReference::write(util::BitWriter &bit_writer) {
    for (auto& checksum: checksums){
        checksum.write(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

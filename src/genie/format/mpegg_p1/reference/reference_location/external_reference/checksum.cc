#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Checksum() {}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Checksum(Algo _algo): checksum_alg(_algo) {}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo Checksum::getType() { return checksum_alg; }

void Checksum::write(util::BitWriter& bit_writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
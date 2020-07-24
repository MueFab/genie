#include "checksum.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Checksum::Checksum(Algo _algo): checksum_alg(_algo) {}

Checksum::Algo Checksum::getType() { return checksum_alg; }

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
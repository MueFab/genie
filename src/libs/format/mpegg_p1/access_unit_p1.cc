#include "access_unit_p1.h"

namespace format {
namespace mpegg_p1 {
AccessUnit::AccessUnit(const format::mpegg_p2::AccessUnit& accessUnit) : format::mpegg_p2::AccessUnit(accessUnit) {}
}  // namespace mpegg_p1
}  // namespace format
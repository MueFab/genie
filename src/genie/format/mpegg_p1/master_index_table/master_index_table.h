#ifndef GENIE_PART1_MIT_H
#define GENIE_PART1_MIT_H

#include "access_unit_info/access_unit_info.h"
#include "u_access_unit_info//u_access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

}
class MasterIndexTable {
   private:
    std::vector<mpegg_p1::MITAccessUnitInfo> ac_info;
    std::vector<mpegg_p1::MITUAccessUnitInfo> u_ac_info;

   public:

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_H

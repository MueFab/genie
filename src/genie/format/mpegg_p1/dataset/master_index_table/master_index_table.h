#ifndef GENIE_PART1_MIT_H
#define GENIE_PART1_MIT_H

#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/u_access_unit_info/u_access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class MasterIndexTable {
   private:
    std::vector<mpegg_p1::MITAccessUnitInfo> ac_info;
    std::vector<mpegg_p1::MITUAccessUnitInfo> u_ac_info;

   public:

    MasterIndexTable();

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;

};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_H

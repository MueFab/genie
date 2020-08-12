#ifndef GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H
#define GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H

#include <vector>
#include <cstdint>
#include <memory>

#include <genie/format/mpegg_p1/dataset/enums.h>


namespace genie {
namespace format {
namespace mpegg_p1 {

class MITUAccessUnitInfo {
   private:

   public:



    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie


#endif  // GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H

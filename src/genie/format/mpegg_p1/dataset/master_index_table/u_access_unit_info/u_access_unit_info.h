#ifndef GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H
#define GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H

#include <cstdint>
#include <vector>
#include <genie/util/bitwriter.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class MITUAccessUnitInfo {
   private:

   public:


    uint64_t getLength() const;
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie


#endif  // GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H

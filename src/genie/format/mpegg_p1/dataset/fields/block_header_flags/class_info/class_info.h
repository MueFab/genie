#ifndef GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H
#define GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H

#include <vector>
#include <cstdint>

#include "genie/core/record/class-type.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ClassInfo{
   private:
    core::record::ClassType clid;
    std::vector<uint8_t> descriptor_IDs;

   public:

    explicit ClassInfo(uint8_t _clid);

    void addDescriptorID(uint8_t _desc_ID);
    void addDescriptorIDs(std::vector<uint8_t>& _desc_IDs);
    void setDescriptorIDs(std::vector<uint8_t>&& _desc_IDs);

    core::record::ClassType getClid() const;

    const std::vector<uint8_t>& getDescriptorIDs() const;

    void write(genie::util::BitWriter& bit_writer, bool block_header_flag) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H

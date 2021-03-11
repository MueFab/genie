#ifndef GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H
#define GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H

#include <cstdint>
#include <vector>
#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>

#include <genie/core/record/class-type.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class ClassInfo{
   private:
    core::record::ClassType clid;
    std::vector<uint8_t> descriptor_IDs;

   public:

    explicit ClassInfo(core::record::ClassType _clid);

    void ReadClassInfo(util::BitReader& reader, bool block_header_flag);

    void addDescriptorID(uint8_t _desc_ID);
    void addDescriptorIDs(std::vector<uint8_t>& _desc_IDs);
    void setDescriptorIDs(std::vector<uint8_t>&& _desc_IDs);

    core::record::ClassType getClid() const;
    uint8_t getNumDescriptors() const;

    uint64_t getBitLength(bool block_header_flag) const;

    void write(util::BitWriter& bit_writer, bool block_header_flag) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_MIT_INFO_CLASS_INFO_H

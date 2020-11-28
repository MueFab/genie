
#include "class_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

ClassInfo::ClassInfo(uint8_t _clid)
    : clid(clid),
      descriptor_IDs(){}

void ClassInfo::addDescriptorID(uint8_t _desc_ID) {
    descriptor_IDs.push_back(_desc_ID);
}

void ClassInfo::addDescriptorIDs(std::vector<uint8_t>& _desc_IDs) {
    std::move(_desc_IDs.begin(), _desc_IDs.end(), std::back_inserter(descriptor_IDs));
}

void ClassInfo::setDescriptorIDs(std::vector<uint8_t>&& _desc_IDs) {
    descriptor_IDs = _desc_IDs;
}

const std::vector<uint8_t>& ClassInfo::getDescriptorIDs() const { return descriptor_IDs.size(); }

void ClassInfo::write(util::BitWriter& bit_writer, bool block_header_flag) const {

    // clid[ci] u(4)
    bit_writer.write(clid, 4);

    if (!block_header_flag){

        // num_descriptors[ci] u(5)
        bit_writer.write(descriptor_IDs.size(), 5);

        for (auto desc_ID: descriptor_IDs){

            // descriptor_ID[ci][di] u(7)
            bit_writer.write(desc_ID, 7);

        }
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
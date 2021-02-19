#include <genie/util/runtime-exception.h>
#include <genie/format/mpegg_p1/util.h>

#include "class_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

ClassInfo::ClassInfo(core::record::ClassType _clid)
    : clid(_clid),
      descriptor_IDs(){}

void ClassInfo::ReadClassInfo(util::BitReader& reader, size_t length, bool block_header_flag) {

    size_t start_pos = reader.getPos();

    /// clid[] u(4)
    clid = reader.read<core::record::ClassType>(4);

    if (!block_header_flag){
        /// num_descriptors[] u(5)
        reader.read<uint8_t>(5);

        for (auto di=0; di < getNumDescriptors(); di++){
            /// descriptors_ID[][] u(7)
            descriptor_IDs.emplace_back(reader.read<uint8_t>(7));
        }
    }

    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid ClassInfo length!");
}

void ClassInfo::addDescriptorID(uint8_t _desc_ID) {
    descriptor_IDs.push_back(_desc_ID);
}

void ClassInfo::addDescriptorIDs(std::vector<uint8_t>& _desc_IDs) {
    std::move(_desc_IDs.begin(), _desc_IDs.end(), std::back_inserter(descriptor_IDs));
}

void ClassInfo::setDescriptorIDs(std::vector<uint8_t>&& _desc_IDs) {
    descriptor_IDs = _desc_IDs;
}

core::record::ClassType ClassInfo::getClid() const { return clid; }
uint8_t ClassInfo::getNumDescriptors() const { return descriptor_IDs.size(); }

uint64_t ClassInfo::getBitLength(bool block_header_flag) const {

    /// clid[] u(4)
    uint64_t bitlen = 4;

    if (!block_header_flag){
        /// num_descriptors[] u(5)
        bitlen += 5;
        /// descriptors_ID[][] u(7)
        bitlen += getNumDescriptors() * 7;
    }
    return bitlen;
}

void ClassInfo::write(util::BitWriter& bit_writer, bool block_header_flag) const {

    // clid[ci] u(4)
    bit_writer.write((uint8_t) clid, 4);

    if (!block_header_flag){

        // num_descriptors[ci] u(5)
        bit_writer.write(getNumDescriptors(), 5);

        for (auto& desc_ID: descriptor_IDs){

            // descriptor_ID[ci][di] u(7)
            bit_writer.write(desc_ID, 7);

        }
    }
}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
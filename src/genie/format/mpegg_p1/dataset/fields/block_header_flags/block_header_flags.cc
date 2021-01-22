#include "genie/util/runtime-exception.h"
#include "block_header_flags.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

BlockConfig::BlockConfig()
    : block_header_flag(false),
      MIT_flag(false){}

BlockConfig::BlockConfig(bool _block_header_flag, bool _mit_flag)
    : block_header_flag(_block_header_flag),
      MIT_flag(_mit_flag){}

bool BlockConfig::getBlockHeaderFlag() const { return block_header_flag; }

bool BlockConfig::getMITFlag() const { return MIT_flag; }

uint8_t BlockConfig::getNumClasses() const { return class_infos.size(); }

const std::vector<ClassInfo>& BlockConfig::getClassInfos() const { return class_infos; }

void BlockConfig::addClassInfo(ClassInfo&& _cls_info) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    class_infos.push_back(std::move(_cls_info));
}

void BlockConfig::addClassInfos(std::vector<ClassInfo>& _cls_infos) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    std::move(_cls_infos.begin(), _cls_infos.end(), std::back_inserter(class_infos));
}

void BlockConfig::setClassInfos(std::vector<ClassInfo>&& _cls_infos) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    class_infos = _cls_infos;
}

uint64_t BlockConfig::getBitLength() const {

        uint64_t bitlen = 1;   /// block_header_flag u(1)
        if (block_header_flag) {
            bitlen += 1; /// MIT_flag u(1)
            bitlen += 1; /// CC_mode_flag u(1)
        }
        else {
            bitlen += 1; /// ordered_blocks_flag u(1)
        }

        bitlen += 4;  /// dataset_type u(4)
        if (MIT_flag) {
            bitlen += 4;  /// num_classes u(4)
            for (auto ci = 0; ci < getNumClasses(); ci++) {
                bitlen += 4;  /// clid[ci] u(4)
                if (!block_header_flag) {
                    bitlen += 5;  /// num_descriptors[ci] u(5)
                    //                for (auto di = 0; di < block_header.getClassInfos()[ci].getDescriptorIds().size(); di++) {
                    //                    bitlen += 7 ;
                    //                }
                    bitlen += getClassInfos()[ci].getDescriptorIDs().size() * 7;  /// descriptors_ID[ci][di] u(7)
                }
            }
        }
        return bitlen;
}

void BlockConfig::write(util::BitWriter& bit_writer) const {

    // block_header_flag u(1)
    bit_writer.write(getBlockHeaderFlag(), 1);
}

void BlockConfig::writeClassInfos(util::BitWriter& bit_writer) const {

    if (MIT_flag){

        // num_classes u(4)
        bit_writer.write(getNumClasses(), 4);

        // clid[ci], num_descriptors[ci], descriptor_ID[ci][di]
        for (auto& class_info: class_infos) {
            class_info.write(bit_writer, getBlockHeaderFlag());
        }
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
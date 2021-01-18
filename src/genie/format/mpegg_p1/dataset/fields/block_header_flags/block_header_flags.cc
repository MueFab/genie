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
    return 1;
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
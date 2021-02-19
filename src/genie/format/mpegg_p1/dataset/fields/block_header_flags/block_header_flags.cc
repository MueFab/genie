#include "block_header_flags.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

BlockConfig::BlockConfig()
    : block_header_flag(false),
      MIT_flag(false),
      class_infos(){}

BlockConfig::BlockConfig(bool _block_header_flag, bool _mit_flag)
    : block_header_flag(_block_header_flag),
      MIT_flag(_mit_flag),
      class_infos(){}

// TODO(Raouf): fix With/Without section in Read/Write and getLength methods
void BlockConfig::ReadBlockConfig(genie::util::BitReader& reader, size_t length) {
    size_t start_pos = reader.getPos();

    /// block_header_flag u(1)
    block_header_flag = reader.read<bool>(1);
/*
    if (block_header_flag) {
        /// MIT_flag, CC_mode_flag
        auto WithHeader_length = reader.read<size_t>();
        ReadWithHeader(reader, WithHeader_length);
    } else {
        /// ordered_blocks_flag
        auto WithoutHeader_length = reader.read<size_t>();
        ReadWithoutHeader(reader, WithoutHeader_length);
    }
*/
    if (MIT_flag) {
        /// num_classes u(4)
        num_classes = reader.read<uint8_t>(4);

        auto ClassInfo_length = reader.read<size_t>();
        for ( auto& info : class_infos) {
            info.ReadClassInfo(reader, ClassInfo_length, block_header_flag);   /// clid[], num_descriptors[], descriptors_ID[][]
        }
    }

    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid ReadBlockConfig length!");
}

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

    uint64_t bitlen = 1;  /// block_header_flag u(1)
/*
    if (block_header_flag) {
        bitlen += with.getBitLength();  /// MIT_flag, CC_mode_flag
    } else {
        bitlen += without.getBitLength();  /// ordered_blocks_flag
    }
*/
    if (MIT_flag) {
        /// num_classes u(4)
        bitlen += 4;

        /// clid[], num_descriptors[], descriptors_ID[][]
        for ( auto& info : class_infos) {
            bitlen += info.getBitLength(block_header_flag);
        }
    }

    return bitlen;
}


void BlockConfig::write(genie::util::BitWriter& bit_writer) const {

    // block_header_flag u(1)
    bit_writer.write(block_header_flag, 1);
/*
    if (block_header_flag) {
        with.write(bit_writer);    /// MIT_flag, CC_mode_flag
    }
    else {
        without.write(bit_writer);  /// ordered_blocks_flag
    }
*/
}

void BlockConfig::writeClassInfos(genie::util::BitWriter& bit_writer) const {

    if (MIT_flag){
        // num_classes u(4)
        bit_writer.write(getNumClasses(), 4);

        // clid[ci], num_descriptors[ci], descriptor_ID[ci][di]
        for (auto& class_info: class_infos) {
            class_info.write(bit_writer, block_header_flag);
        }
    }
}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
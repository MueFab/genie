/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/fields/block_header_flags/block_header_flags.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

BlockConfig::BlockConfig()
    : block_header_flag(false),
      MIT_flag(false),
      CC_mode_flag(false),
      ordered_blocks_flag(false),
      class_infos(),
      num_classes() {}

// ---------------------------------------------------------------------------------------------------------------------

BlockConfig::BlockConfig(bool _block_header_flag, bool _mit_flag)
    : block_header_flag(_block_header_flag),
      MIT_flag(_mit_flag),
      CC_mode_flag(false),
      ordered_blocks_flag(false),
      class_infos(),
      num_classes() {}

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::ReadBlockConfig(genie::util::BitReader& reader) {
    /// block_header_flag u(1)
    block_header_flag = reader.read<bool>(1);

    if (block_header_flag) {
        /// MIT_flag u(1), CC_mode_flag u(1)
        MIT_flag = reader.read<bool>(1);
        CC_mode_flag = reader.read<bool>(1);

    } else {
        /// ordered_blocks_flag u(1)
        ordered_blocks_flag = reader.read<bool>(1);
    }

    if (MIT_flag) {
        /// num_classes u(4)
        num_classes = reader.read<uint8_t>(4);

        for (auto& info : class_infos) {
            /// clid[], num_descriptors[], descriptors_ID[][]
            info.ReadClassInfo(reader, block_header_flag);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockConfig::getBlockHeaderFlag() const { return block_header_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockConfig::getMITFlag() const { return MIT_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BlockConfig::getNumClasses() const { return class_infos.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<ClassInfo>& BlockConfig::getClassInfos() const { return class_infos; }

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::addClassInfo(ClassInfo&& _cls_info) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    class_infos.push_back(std::move(_cls_info));
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::addClassInfos(std::vector<ClassInfo>& _cls_infos) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    std::move(_cls_infos.begin(), _cls_infos.end(), std::back_inserter(class_infos));
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::setClassInfos(std::vector<ClassInfo>&& _cls_infos) {
    UTILS_DIE_IF(!MIT_flag, "Adding class_info but MIT_flag is false");

    class_infos = _cls_infos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BlockConfig::getBitLength() const {
    uint64_t bitlen = 1;  /// block_header_flag u(1)

    if (block_header_flag) {
        bitlen += 2;  /// MIT_flag u(1), CC_mode_flag u(1)
    } else {
        bitlen += 1;  /// ordered_blocks_flag u(1)
    }

    if (MIT_flag) {
        /// num_classes u(4)
        bitlen += 4;

        /// clid[], num_descriptors[], descriptors_ID[][]
        for (auto& info : class_infos) {
            bitlen += info.getBitLength(block_header_flag);
        }
    }

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::write(genie::util::BitWriter& bit_writer) const {
    // block_header_flag u(1)
    bit_writer.write(block_header_flag, 1);

    if (block_header_flag) {
        /// MIT_flag, CC_mode_flag
        bit_writer.write(MIT_flag, 1);
        bit_writer.write(CC_mode_flag, 1);
    } else {
        bit_writer.write(ordered_blocks_flag, 1);  /// ordered_blocks_flag
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockConfig::writeClassInfos(genie::util::BitWriter& bit_writer) const {
    if (MIT_flag) {
        // num_classes u(4)
        bit_writer.write(getNumClasses(), 4);

        // clid[ci], num_descriptors[ci], descriptor_ID[ci][di]
        for (auto& info : class_infos) {
            info.write(bit_writer, block_header_flag);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

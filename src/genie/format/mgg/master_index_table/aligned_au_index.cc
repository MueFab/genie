/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include "genie/format/mgg/master_index_table/aligned_au_index.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace master_index_table {

// ---------------------------------------------------------------------------------------------------------------------

bool AlignedAUIndex::operator==(const AlignedAUIndex& other) const {
    return au_byte_offset == other.au_byte_offset && au_start_position == other.au_start_position &&
           au_end_position == other.au_end_position && ref_cfg == other.ref_cfg && extended_cfg == other.extended_cfg &&
           block_byte_offset == other.block_byte_offset && byte_offset_size == other.byte_offset_size &&
           position_size == other.position_size;
}

// ---------------------------------------------------------------------------------------------------------------------

AlignedAUIndex::AlignedAUIndex(uint64_t _au_byte_offset, uint64_t _au_start_position, uint64_t _au_end_position,
                               uint8_t _byte_offset_size, uint8_t _position_size)
    : au_byte_offset(_au_byte_offset),
      au_start_position(_au_start_position),
      au_end_position(_au_end_position),
      byte_offset_size(_byte_offset_size),
      position_size(_position_size) {}

// ---------------------------------------------------------------------------------------------------------------------

AlignedAUIndex::AlignedAUIndex(genie::util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                               core::parameter::DataUnit::DatasetType dataset_type, bool multiple_alignment,
                               bool block_header_flag, const std::vector<genie::core::GenDesc>& descriptors)
    : byte_offset_size(_byte_offset_size), position_size(_position_size) {
    au_byte_offset = reader.read<uint64_t>(byte_offset_size);
    au_start_position = reader.read<uint64_t>(position_size);
    au_end_position = reader.read<uint64_t>(position_size);
    if (dataset_type == core::parameter::DataUnit::DatasetType::REFERENCE) {
        ref_cfg = genie::format::mgb::RefCfg(position_size, reader);
    }
    if (multiple_alignment) {
        extended_cfg = genie::format::mgb::ExtendedAu(position_size, reader);
    }
    if (!block_header_flag) {
        for (const auto& d : descriptors) {
            (void)d;
            block_byte_offset.emplace_back(reader.read<uint64_t>(byte_offset_size));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::write(genie::util::BitWriter& writer) const {
    writer.write(au_byte_offset, byte_offset_size);
    writer.write(au_start_position, position_size);
    writer.write(au_end_position, position_size);
    if (ref_cfg != std::nullopt) {
        ref_cfg->write(writer);
    }
    if (extended_cfg != std::nullopt) {
        extended_cfg->write(writer);
    }
    for (const auto& b : block_byte_offset) {
        writer.write(b, byte_offset_size);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AlignedAUIndex::getByteOffset() const { return au_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AlignedAUIndex::getAUStartPos() const { return au_start_position; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AlignedAUIndex::getAUEndPos() const { return au_end_position; }

// ---------------------------------------------------------------------------------------------------------------------

bool AlignedAUIndex::isReference() const { return ref_cfg != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::RefCfg& AlignedAUIndex::getReferenceInfo() const { return *ref_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg) { ref_cfg = std::move(_ref_cfg); }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::setExtended(genie::format::mgb::ExtendedAu _ext_au) { extended_cfg = std::move(_ext_au); }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::ExtendedAu& AlignedAUIndex::getExtension() const { return *extended_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

bool AlignedAUIndex::isExtended() const { return extended_cfg != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& AlignedAUIndex::getBlockOffsets() const { return block_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::addBlockOffset(uint64_t offset) {
    UTILS_DIE_IF(!block_byte_offset.empty() && block_byte_offset.back() > offset, "Blocks unordered");
    block_byte_offset.emplace_back(offset);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace master_index_table
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

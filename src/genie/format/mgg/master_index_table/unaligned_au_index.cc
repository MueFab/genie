/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/master_index_table/unaligned_au_index.h"
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::master_index_table {

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::operator==(const UnalignedAUIndex& other) const {
    return au_byte_offset == other.au_byte_offset && ref_cfg == other.ref_cfg && sig_cfg == other.sig_cfg &&
           block_byte_offset == other.block_byte_offset && byte_offset_size == other.byte_offset_size &&
           position_size == other.position_size && signature_size == other.signature_size;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& UnalignedAUIndex::getBlockOffsets() const { return block_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

UnalignedAUIndex::UnalignedAUIndex(util::BitReader& reader, const uint8_t _byte_offset_size,
                                   const uint8_t _position_size,
                                   const core::parameter::DataUnit::DatasetType dataset_type, const bool signature_flag,
                                   const bool signature_const_flag, const uint8_t _signature_size,
                                   const bool block_header_flag, const std::vector<core::GenDesc>& descriptors,
                                   const core::AlphabetID alphabet)
    : byte_offset_size(_byte_offset_size), position_size(_position_size), signature_size(_signature_size) {
    au_byte_offset = reader.read<uint64_t>(byte_offset_size);
    if (dataset_type == core::parameter::DataUnit::DatasetType::REFERENCE) {
        ref_cfg = mgb::RefCfg(position_size, reader);
    } else if (signature_flag) {
        sig_cfg = mgb::SignatureCfg(reader, signature_const_flag ? signature_size : 0,
                                    getAlphabetProperties(alphabet).base_bits);
        reader.flushHeldBits();
    }
    if (!block_header_flag) {
        for (const auto& d : descriptors) {
            (void)d;
            block_byte_offset.emplace_back(reader.read<uint64_t>(byte_offset_size));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::write(util::BitWriter& writer) const {
    writer.writeBits(au_byte_offset, byte_offset_size);
    if (ref_cfg != std::nullopt) {
        ref_cfg->write(writer);
    }
    if (sig_cfg != std::nullopt) {
        sig_cfg->write(writer);
        writer.flushBits();
    }
    for (const auto& b : block_byte_offset) {
        writer.writeBits(b, byte_offset_size);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t UnalignedAUIndex::getAUOffset() const { return au_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::addBlockOffset(uint64_t offset) {
    UTILS_DIE_IF(!block_byte_offset.empty() && block_byte_offset.back() > offset, "Blocks unordered");
    block_byte_offset.emplace_back(offset);
}

// ---------------------------------------------------------------------------------------------------------------------

UnalignedAUIndex::UnalignedAUIndex(const uint64_t _au_byte_offset, const uint8_t _byte_offset_size,
                                   const int8_t _position_size, const int8_t _signature_size)
    : au_byte_offset(_au_byte_offset),
      byte_offset_size(_byte_offset_size),
      position_size(_position_size),
      signature_size(_signature_size) {}

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::isReference() const { return ref_cfg != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const mgb::RefCfg& UnalignedAUIndex::getReferenceInfo() const { return *ref_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::setReferenceInfo(const mgb::RefCfg& _ref_cfg) { ref_cfg = _ref_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::hasSignature() const { return sig_cfg != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const mgb::SignatureCfg& UnalignedAUIndex::getSignatureInfo() const { return *sig_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::setSignatureInfo(const mgb::SignatureCfg& sigcfg) { sig_cfg = sigcfg; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::master_index_table

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

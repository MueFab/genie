/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include "genie/format/mgg/master_index_table/unaligned_au_index.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace master_index_table {

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::operator==(const UnalignedAUIndex& other) const {
    return au_byte_offset == other.au_byte_offset && ref_cfg == other.ref_cfg && sig_cfg == other.sig_cfg &&
           block_byte_offset == other.block_byte_offset && byte_offset_size == other.byte_offset_size &&
           position_size == other.position_size && signature_size == other.signature_size;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& UnalignedAUIndex::getBlockOffsets() const { return block_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

UnalignedAUIndex::UnalignedAUIndex(util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                                   core::parameter::DataUnit::DatasetType dataset_type, bool signature_flag,
                                   bool signature_const_flag, uint8_t _signature_size, bool block_header_flag,
                                   const std::vector<genie::core::GenDesc>& descriptors, core::AlphabetID alphabet)
    : byte_offset_size(_byte_offset_size), position_size(_position_size), signature_size(_signature_size) {
    au_byte_offset = reader.read<uint64_t>(byte_offset_size);
    if (dataset_type == core::parameter::DataUnit::DatasetType::REFERENCE) {
        ref_cfg = genie::format::mgb::RefCfg(position_size, reader);
    } else if (signature_flag) {
        sig_cfg = genie::format::mgb::SignatureCfg(reader, signature_const_flag ? signature_size : 0,
                                                   core::getAlphabetProperties(alphabet).base_bits);
        reader.flush();
    }
    if (!block_header_flag) {
        for (const auto& d : descriptors) {
            (void)d;
            block_byte_offset.emplace_back(reader.read<uint64_t>(byte_offset_size));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::write(genie::util::BitWriter& writer) const {
    writer.write(au_byte_offset, byte_offset_size);
    if (ref_cfg != boost::none) {
        ref_cfg->write(writer);
    }
    if (sig_cfg != boost::none) {
        sig_cfg->write(writer);
        writer.flush();
    }
    for (const auto& b : block_byte_offset) {
        writer.write(b, byte_offset_size);
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

UnalignedAUIndex::UnalignedAUIndex(uint64_t _au_byte_offset, uint8_t _byte_offset_size, int8_t _position_size,
                                   int8_t _signature_size)
    : au_byte_offset(_au_byte_offset),
      byte_offset_size(_byte_offset_size),
      position_size(_position_size),
      signature_size(_signature_size) {}

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::isReference() const { return ref_cfg != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::RefCfg& UnalignedAUIndex::getReferenceInfo() const { return *ref_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg) { ref_cfg = std::move(_ref_cfg); }

// ---------------------------------------------------------------------------------------------------------------------

bool UnalignedAUIndex::hasSignature() const { return sig_cfg != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::SignatureCfg& UnalignedAUIndex::getSignatureInfo() const { return *sig_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void UnalignedAUIndex::setSignatureInfo(genie::format::mgb::SignatureCfg sigcfg) { sig_cfg = std::move(sigcfg); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace master_index_table
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

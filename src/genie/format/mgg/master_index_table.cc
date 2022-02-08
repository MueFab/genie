/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/master_index_table.h"
#include <sstream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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
    if (ref_cfg != boost::none) {
        ref_cfg->write(writer);
    }
    if (extended_cfg != boost::none) {
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

bool AlignedAUIndex::isReference() const { return ref_cfg != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::RefCfg& AlignedAUIndex::getReferenceInfo() const { return *ref_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg) { ref_cfg = std::move(_ref_cfg); }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::setExtended(genie::format::mgb::ExtendedAu _ext_au) { extended_cfg = std::move(_ext_au); }

// ---------------------------------------------------------------------------------------------------------------------

const genie::format::mgb::ExtendedAu& AlignedAUIndex::getExtension() const { return *extended_cfg; }

// ---------------------------------------------------------------------------------------------------------------------

bool AlignedAUIndex::isExtended() const { return extended_cfg != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& AlignedAUIndex::getBlockOffsets() const { return block_byte_offset; }

// ---------------------------------------------------------------------------------------------------------------------

void AlignedAUIndex::addBlockOffset(uint64_t offset) {
    UTILS_DIE_IF(!block_byte_offset.empty() && block_byte_offset.back() > offset, "Blocks unordered");
    block_byte_offset.emplace_back(offset);
}

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

bool MasterIndexTable::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const MasterIndexTable&>(info);
    return aligned_aus == other.aligned_aus && unaligned_aus == other.unaligned_aus;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<std::vector<AlignedAUIndex>>>& MasterIndexTable::getAlignedAUs() const {
    return aligned_aus;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<UnalignedAUIndex>& MasterIndexTable::getUnalignedAUs() const { return unaligned_aus; }

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(uint16_t seq_count, uint8_t num_classes) {
    aligned_aus.resize(seq_count, std::vector<std::vector<AlignedAUIndex>>(num_classes, std::vector<AlignedAUIndex>()));
}

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.read<uint64_t>();
    aligned_aus.resize(
        hdr.getReferenceOptions().getSeqIDs().size(),
        std::vector<std::vector<AlignedAUIndex>>(hdr.getMITConfigs().size(), std::vector<AlignedAUIndex>()));
    for (size_t seq = 0; seq < hdr.getReferenceOptions().getSeqIDs().size(); ++seq) {
        for (size_t ci = 0; ci < hdr.getMITConfigs().size(); ++ci) {
            if (core::record::ClassType::CLASS_U == hdr.getMITConfigs()[ci].getClassID()) {
                continue;
            }
            for (size_t au_id = 0; au_id < hdr.getReferenceOptions().getSeqBlocks()[seq]; ++au_id) {
                aligned_aus[seq][ci].emplace_back(reader, hdr.getByteOffsetSize(), hdr.getPosBits(),
                                                  hdr.getDatasetType(), hdr.getMultipleAlignmentFlag(),
                                                  hdr.isBlockHeaderEnabled(),
                                                  hdr.getMITConfigs()[ci].getDescriptorIDs());
            }
        }
    }
    for (size_t uau_id = 0; uau_id < hdr.getNumUAccessUnits(); ++uau_id) {
        unaligned_aus.emplace_back(
            reader, hdr.getByteOffsetSize(), hdr.getPosBits(), hdr.getDatasetType(), hdr.getUOptions().hasSignature(),
            hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength(),
            hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength()
                ? hdr.getUOptions().getSignature().getConstLength()
                : static_cast<uint8_t>(0),
            hdr.isBlockHeaderEnabled(), hdr.getMITConfigs().back().getDescriptorIDs(), hdr.getAlphabetID());
    }
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void MasterIndexTable::box_write(genie::util::BitWriter& bitWriter) const {
    for (const auto& a : aligned_aus) {
        for (const auto& b : a) {
            for (const auto& c : b) {
                c.write(bitWriter);
            }
        }
    }
    for (const auto& a : unaligned_aus) {
        a.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& MasterIndexTable::getKey() const {
    static const std::string key = "mitb";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint64_t> MasterIndexTable::getDescriptorStreamOffsets(uint8_t class_index, uint8_t desc_index,
                                                                   bool isUnaligned, uint64_t total_size) const {
    std::vector<uint64_t> offsets;
    if (isUnaligned) {
        for (const auto& unaligned_au : unaligned_aus) {
            offsets.emplace_back(unaligned_au.getBlockOffsets().at(desc_index));
        }
    } else {
        for (const auto& seq : aligned_aus) {
            for (const auto& aligned_au : seq.at(class_index)) {
                offsets.emplace_back(aligned_au.getBlockOffsets().at(desc_index));
            }
        }
    }
    std::sort(offsets.begin(), offsets.end());
    uint64_t last = offsets.front();
    offsets.erase(offsets.begin());
    offsets.emplace_back(total_size + last);
    for (uint64_t& offset : offsets) {
        uint64_t tmp = offset;
        offset = offset - last;
        last = tmp;
    }
    return offsets;
}

// ---------------------------------------------------------------------------------------------------------------------

void MasterIndexTable::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Master index table");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

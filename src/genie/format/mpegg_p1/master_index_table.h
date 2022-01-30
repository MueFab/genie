//
// Created by fabian on 1/29/22.
//

#ifndef GENIE_MASTER_INDEX_TABLE_H
#define GENIE_MASTER_INDEX_TABLE_H

#include <boost/optional/optional.hpp>
#include <sstream>
#include <vector>
#include "dataset_header.h"
#include "gen_info.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/extended_au.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/runtime-exception.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class AlignedAUIndex {
 private:
    uint64_t au_byte_offset;
    uint64_t au_start_position;
    uint64_t au_end_position;
    boost::optional<genie::format::mgb::RefCfg> ref_cfg;
    boost::optional<genie::format::mgb::ExtendedAu> extended_cfg;
    std::vector<uint64_t> block_byte_offset;

    uint8_t byte_offset_size;
    uint8_t position_size;

 public:
    AlignedAUIndex(uint64_t _au_byte_offset, uint64_t _au_start_position, uint64_t _au_end_position,
                   uint8_t _byte_offset_size, uint8_t _position_size)
        : au_byte_offset(_au_byte_offset),
          au_start_position(_au_start_position),
          au_end_position(_au_end_position),
          byte_offset_size(_byte_offset_size),
          position_size(_position_size) {}

    AlignedAUIndex(genie::util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                   core::parameter::DataUnit::DatasetType dataset_type, bool multiple_alignment, bool block_header_flag,
                   const std::vector<genie::core::GenDesc>& descriptors)
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
                block_byte_offset.emplace_back(reader.read<uint64_t>(byte_offset_size));
            }
        }
    }

    void write(genie::util::BitWriter& writer) const {
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

    uint64_t getByteOffset() const { return au_byte_offset; }

    uint64_t getAUStartPos() const { return au_start_position; }

    uint64_t getAUEndPos() const { return au_end_position; }

    bool isReference() const { return ref_cfg != boost::none; }

    const genie::format::mgb::RefCfg& getReferenceInfo() const { return *ref_cfg; }

    void setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg) { ref_cfg = std::move(_ref_cfg); }

    void setExtended(genie::format::mgb::ExtendedAu _ext_au) { extended_cfg = std::move(_ext_au); }

    const genie::format::mgb::ExtendedAu& getExtension() const { return *extended_cfg; }

    bool isExtended() const { return extended_cfg != boost::none; }

    const std::vector<uint64_t>& getBlockOffsets() const { return block_byte_offset; }

    void addBlockOffset(uint64_t offset) {
        UTILS_DIE_IF(!block_byte_offset.empty() && block_byte_offset.back() > offset, "Blocks unordered");
        block_byte_offset.emplace_back(offset);
    }

};

class UnalignedAUIndex {
 private:
    uint64_t au_byte_offset;
    boost::optional<genie::format::mgb::RefCfg> ref_cfg;
    boost::optional<genie::format::mgb::SignatureCfg> sig_cfg;
    std::vector<uint64_t> block_byte_offset;

    uint8_t byte_offset_size;
    uint8_t position_size;
    uint8_t signature_size;

 public:

    const std::vector<uint64_t>& getBlockOffsets() const {
        return block_byte_offset;
    }

    explicit UnalignedAUIndex(util::BitReader& reader, uint8_t _byte_offset_size, uint8_t _position_size,
                              core::parameter::DataUnit::DatasetType dataset_type, bool signature_flag,
                              bool signature_const_flag, uint8_t _signature_size, bool block_header_flag,
                              const std::vector<genie::core::GenDesc>& descriptors)
        : byte_offset_size(_byte_offset_size), position_size(_position_size), signature_size(_signature_size) {
        au_byte_offset = reader.read<uint64_t>(byte_offset_size);
        if (dataset_type == core::parameter::DataUnit::DatasetType::REFERENCE) {
            ref_cfg = genie::format::mgb::RefCfg(position_size, reader);
        } else if (signature_flag) {
            sig_cfg = genie::format::mgb::SignatureCfg(reader, signature_const_flag ? signature_size : 0);
            reader.flush();
        }
        if (!block_header_flag) {
            for (const auto& d : descriptors) {
                block_byte_offset.emplace_back(reader.read<uint64_t>(byte_offset_size));
            }
        }
    }

    void write(genie::util::BitWriter& writer) const {
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

    uint64_t getAUOffset() const { return au_byte_offset; }

    void addBlockOffset(uint64_t offset) {
        UTILS_DIE_IF(!block_byte_offset.empty() && block_byte_offset.back() > offset, "Blocks unordered");
        block_byte_offset.emplace_back(offset);
    }

    UnalignedAUIndex(uint64_t _au_byte_offset, uint8_t _byte_offset_size, int8_t _position_size, int8_t _signature_size)
        : au_byte_offset(_au_byte_offset),
          byte_offset_size(_byte_offset_size),
          position_size(_position_size),
          signature_size(_signature_size) {}

    bool isReference() const { return ref_cfg != boost::none; }
    const genie::format::mgb::RefCfg& getReferenceInfo() const { return *ref_cfg; }

    void setReferenceInfo(genie::format::mgb::RefCfg _ref_cfg) { ref_cfg = std::move(_ref_cfg); }

    bool hasSignature() const { return sig_cfg != boost::none; }

    const genie::format::mgb::SignatureCfg& getSignatureInfo() const { return *sig_cfg; }

    void setSignatureInfo(genie::format::mgb::SignatureCfg sigcfg) { sig_cfg = std::move(sigcfg); }
};

/**
 * @brief
 */
class MasterIndexTable : public GenInfo {
 private:
    std::vector<std::vector<std::vector<AlignedAUIndex>>> aligned_aus;
    std::vector<UnalignedAUIndex> unaligned_aus;

 public:

    const std::vector<std::vector<std::vector<AlignedAUIndex>>>& getAlignedAUs() const {
        return aligned_aus;
    }

    const std::vector<UnalignedAUIndex>& getUnalignedAUs() const {
        return unaligned_aus;
    }

    MasterIndexTable(uint16_t seq_count, uint8_t num_classes) {
        aligned_aus.resize(seq_count,
                           std::vector<std::vector<AlignedAUIndex>>(num_classes, std::vector<AlignedAUIndex>()));
    }

    MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr) {
        reader.read<uint64_t>();
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
                reader, hdr.getByteOffsetSize(), hdr.getPosBits(), hdr.getDatasetType(),
                hdr.getUOptions().hasSignature(),
                hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength(),
                hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength()
                    ? hdr.getUOptions().getSignature().getConstLength()
                    : 0,
                hdr.isBlockHeaderEnabled(), hdr.getMITConfigs().back().getDescriptorIDs());
        }
    }

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
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

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "mitb";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_MASTER_INDEX_TABLE_H

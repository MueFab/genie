/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <boost/optional/optional.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

class BlockHeaderOnOptions {
 private:
    bool mit_flag;
    bool cc_mode_flag;

 public:
    BlockHeaderOnOptions(bool _mit_flag, bool _cc_mode_flag) : mit_flag(_mit_flag), cc_mode_flag(_cc_mode_flag) {}

    explicit BlockHeaderOnOptions(genie::util::BitReader& reader) {
        mit_flag = reader.read<bool>(1);
        cc_mode_flag = reader.read<bool>(1);
    }

    void write(genie::util::BitWriter& writer) const {
        writer.write(mit_flag, 1);
        writer.write(cc_mode_flag, 1);
    }

    bool getMITFlag() const { return mit_flag; }

    bool getCCFlag() const { return cc_mode_flag; }
};

struct BlockHeaderOffOptions {
 private:
    bool ordered_blocks_flag;

 public:
    explicit BlockHeaderOffOptions(bool _ordered_blocks_flag) : ordered_blocks_flag(_ordered_blocks_flag) {}

    explicit BlockHeaderOffOptions(util::BitReader& reader) { ordered_blocks_flag = reader.read<bool>(1); }

    bool getOrderedBlocksFlag() const { return ordered_blocks_flag; }

    void write(genie::util::BitWriter& writer) const { writer.write(ordered_blocks_flag, 1); }
};

class ReferenceOptions {
 private:
    uint8_t reference_ID;
    std::vector<uint16_t> seq_ID;
    std::vector<uint32_t> seq_blocks;

 public:
    explicit ReferenceOptions(genie::util::BitReader& reader) {
        auto seq_count = reader.read<uint16_t>();
        if (!seq_count) {
            reference_ID = std::numeric_limits<uint8_t>::max();
            return;
        }
        reference_ID = reader.read<uint8_t>();
        for (uint16_t i = 0; i < seq_count; ++i) {
            seq_ID.emplace_back(reader.read<uint16_t>());
        }
        for (uint16_t i = 0; i < seq_count; ++i) {
            seq_blocks.emplace_back(reader.read<uint32_t>());
        }
    }

    void write(genie::util::BitWriter& writer) const {
        writer.write(seq_ID.size(), 16);
        if (seq_ID.empty()) {
            return;
        }
        writer.write(reference_ID, 8);

        for (auto& i : seq_ID) {
            writer.write(i, 16);
        }

        for (auto& b : seq_blocks) {
            writer.write(b, 32);
        }
    }

    explicit ReferenceOptions() : reference_ID(std::numeric_limits<uint8_t>::max()) {}

    void addSeq(uint8_t _reference_ID, uint8_t _seq_id, uint16_t blocks) {
        UTILS_DIE_IF(_reference_ID != reference_ID && !seq_ID.empty(), "Unmatching ref id");
        reference_ID = _reference_ID;
        seq_ID.push_back(_seq_id);
        seq_blocks.push_back(blocks);
    }

    const std::vector<uint16_t>& getSeqIDs() const { return seq_ID; }

    const std::vector<uint32_t>& getSeqBlocks() const { return seq_blocks; }

    uint8_t getReferenceID() const { return reference_ID; }
};

class MITClassConfig {
 private:
    genie::core::record::ClassType id;
    std::vector<genie::core::GenDesc> descriptor_ids;

 public:
    explicit MITClassConfig(genie::core::record::ClassType _id) : id(_id) {}

    MITClassConfig(genie::util::BitReader& reader, bool block_header_flag) {
        id = reader.read<genie::core::record::ClassType>(4);
        if (!block_header_flag) {
            auto num_descriptors = reader.read<uint8_t>(5);
            for (size_t i = 0; i < num_descriptors; ++i) {
                descriptor_ids.emplace_back(reader.read<genie::core::GenDesc>(7));
            }
        }
    }

    void write(genie::util::BitWriter& writer) const {
        writer.write(static_cast<uint8_t>(id), 4);
        if (!descriptor_ids.empty()) {
            writer.write(descriptor_ids.size(), 5);
            for (const auto& d : descriptor_ids) {
                writer.write(static_cast<uint8_t>(d), 7);
            }
        }
    }

    void addDescriptorID(genie::core::GenDesc desc) { descriptor_ids.emplace_back(desc); }

    genie::core::record::ClassType getClassID() const { return id; }

    const std::vector<genie::core::GenDesc>& getDescriptorIDs() const { return descriptor_ids; }
};

class USignature {
 private:
    boost::optional<uint8_t> const_length;

 public:
    USignature() : const_length(boost::none) {}

    explicit USignature(uint8_t _const_length) : const_length(_const_length) {}

    explicit USignature(genie::util::BitReader& reader) {
        bool U_signature_constant_length = reader.read<bool>(1);
        if (U_signature_constant_length) {
            const_length = reader.read<uint8_t>(8);
        }
    }

    void write(genie::util::BitWriter& writer) const {
        writer.write(isConstLength(), 1);
        if (isConstLength()) {
            writer.write(getConstLength(), 8);
        }
    }

    bool isConstLength() const { return const_length != boost::none; }

    uint8_t getConstLength() const { return *const_length; }
};

class UOptions {
 private:
    uint64_t reserved1;
    boost::optional<USignature> u_signature;
    boost::optional<uint8_t> reserved2;
    bool reserved3;

 public:
    explicit UOptions(uint64_t _reserved1 = 0, bool _reserved3 = false)
        : reserved1(_reserved1), u_signature(boost::none), reserved2(boost::none), reserved3(_reserved3) {}

    explicit UOptions(genie::util::BitReader& reader) {
        reserved1 = reader.read<uint64_t>(62);
        bool U_signature_flag = reader.read<bool>(1);
        if (U_signature_flag) {
            u_signature = USignature(reader);
        }
        bool reserved_flag = reader.read<bool>(1);
        if (reserved_flag) {
            reserved2 = reader.read<uint8_t>(8);
        }
        reserved3 = reader.read<bool>(1);
    }

    void write(genie::util::BitWriter& writer) const {
        writer.write(reserved1, 62);
        writer.write(hasSignature(), 1);
        if (hasSignature()) {
            u_signature->write(writer);
        }
        writer.write(hasReserved2(), 1);
        if (hasReserved2()) {
            writer.write(getReserved2(), 8);
        }
        writer.write(reserved3, 1);
    }

    uint64_t getReserved1() const { return reserved1; }

    bool getReserved3() const { return reserved3; }

    bool hasReserved2() const { return reserved2 != boost::none; }

    uint8_t getReserved2() const { return *reserved2; }

    bool hasSignature() const { return u_signature != boost::none; }

    const USignature& getSignature() const { return *u_signature; }

    void addSignature(USignature s) { u_signature = s; }

    void addReserved2(uint8_t r) { reserved2 = r; }
};

/**
 * @brief
 */
class DatasetHeader : public GenInfo {
 public:
 private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2 table 19
     *  ------------------------------------------------------------------------------------------------------------ */
    uint8_t group_ID;
    uint16_t ID;
    genie::core::MPEGMinorVersion version;
    bool multiple_alignment_flag;
    bool byte_offset_size_flag;
    bool non_overlapping_AU_range_flag;
    bool pos_40_bits_flag;

    boost::optional<BlockHeaderOnOptions> block_header_on;
    boost::optional<BlockHeaderOffOptions> block_header_off;

    ReferenceOptions referenceOptions;

    core::parameter::DataUnit::DatasetType dataset_type;

    std::vector<MITClassConfig> mit_configs;
    bool parameters_update_flag;
    core::AlphabetID alphabet_id;

    uint32_t num_U_access_units;
    boost::optional<UOptions> u_options;

    std::vector<boost::optional<uint32_t>> thresholds;

 public:
    uint8_t getDatasetGroupID() const { return group_ID; }

    uint16_t getDatasetID() const { return ID; }

    genie::core::MPEGMinorVersion getVersion() const { return version; }

    bool getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

    uint8_t getByteOffset() const { return byte_offset_size_flag ? 64 : 32; }

    bool getNonOverlappingAURangeFlag() const { return non_overlapping_AU_range_flag; }

    uint8_t getPosBits() const { return pos_40_bits_flag ? 40 : 32; }

    bool isBlockHeaderEnabled() const { return block_header_on != boost::none; }

    bool isMITEnabled() const { return block_header_off != boost::none || block_header_on->getMITFlag(); }

    bool isCCModeEnabled() const { return block_header_on != boost::none && block_header_on->getCCFlag(); };

    bool isOrderedBlockMode() const {
        return block_header_off != boost::none && block_header_off->getOrderedBlocksFlag();
    }

    const ReferenceOptions& getReferenceOptions() const { return referenceOptions; }

    core::parameter::DataUnit::DatasetType getDatasetType() const { return dataset_type; }

    const std::vector<MITClassConfig>& getMITConfigs() const { return mit_configs; }

    bool getParameterUpdateFlag() const { return parameters_update_flag; }

    core::AlphabetID getAlphabetID() const { return alphabet_id; }

    uint32_t getNumUAccessUnits() const { return num_U_access_units; }

    const UOptions& getUOptions() const { return *u_options; }

    const std::vector<boost::optional<uint32_t>>& getRefSeqThresholds() const { return thresholds; }

    const std::string& getKey() const override {
        static const std::string key = "dthd";
        return key;
    }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    DatasetHeader(uint8_t _dataset_group_id, uint16_t _dataset_id, genie::core::MPEGMinorVersion _version,
                  bool _multiple_alignments_flags, bool _byte_offset_size_flags, bool _non_overlapping_AU_range_flag,
                  bool _pos_40_bits_flag, core::parameter::DataUnit::DatasetType _dataset_type,
                  bool _parameters_update_flag, core::AlphabetID _alphabet_id)
        : group_ID(_dataset_group_id),
          ID(_dataset_id),
          version(_version),
          multiple_alignment_flag(_multiple_alignments_flags),
          byte_offset_size_flag(_byte_offset_size_flags),
          non_overlapping_AU_range_flag(_non_overlapping_AU_range_flag),
          pos_40_bits_flag(_pos_40_bits_flag),
          dataset_type(_dataset_type),
          parameters_update_flag(_parameters_update_flag),
          alphabet_id(_alphabet_id) {
        block_header_on = BlockHeaderOnOptions{false, false};
        num_U_access_units = 0;
    }

    explicit DatasetHeader(genie::util::BitReader& reader) {
        reader.readBypassBE<uint64_t>();
        group_ID = reader.readBypassBE<uint8_t>();
        ID = reader.readBypassBE<uint16_t>();
        std::string versionString(4, '\0');
        reader.readBypass(versionString);
        version = core::getMPEGVersion(versionString);
        UTILS_DIE_IF(version == core::MPEGMinorVersion::UNKNOWN, "Unknown MPEG version");

        multiple_alignment_flag = reader.read<bool>(1);
        byte_offset_size_flag = reader.read<bool>(1);
        non_overlapping_AU_range_flag = reader.read<bool>(1);
        pos_40_bits_flag = reader.read<bool>(1);
        bool block_header_flag = reader.read<bool>(1);
        if (block_header_flag) {
            block_header_on = BlockHeaderOnOptions(reader);
        } else {
            block_header_off = BlockHeaderOffOptions(reader);
        }
        referenceOptions = ReferenceOptions(reader);
        dataset_type = reader.read<genie::core::parameter::DataUnit::DatasetType>(4);
        if ((block_header_on != boost::none && block_header_on->getMITFlag()) || block_header_on == boost::none) {
            auto num_classes = reader.read<uint8_t>(4);
            for (size_t i = 0; i < num_classes; ++i) {
                mit_configs.emplace_back(reader, block_header_flag);
            }
        }
        parameters_update_flag = reader.read<bool>(1);
        alphabet_id = reader.read<genie::core::AlphabetID>(7);
        num_U_access_units = reader.read<uint32_t>(32);
        if (num_U_access_units) {
            u_options = UOptions(reader);
        }
        for (size_t i = 0; i < referenceOptions.getSeqIDs().size(); ++i) {
            bool flag = reader.read<bool>(1);
            if (flag) {
                thresholds.emplace_back(reader.read<uint32_t>(31));
            } else {
                thresholds.emplace_back(boost::none);
            }
        }
        reader.flush();
    }

    void write(genie::util::BitWriter& writer) const override {
        GenInfo::write(writer);
        writer.writeBypassBE<uint8_t>(group_ID);
        writer.writeBypassBE<uint16_t>(ID);
        const auto& v_string = genie::core::getMPEGVersionString(version);
        writer.writeBypass(v_string.data(), v_string.length());

        writer.write(multiple_alignment_flag, 1);
        writer.write(byte_offset_size_flag, 1);
        writer.write(non_overlapping_AU_range_flag, 1);
        writer.write(pos_40_bits_flag, 1);
        writer.write(block_header_on != boost::none, 1);
        if (block_header_on != boost::none) {
            block_header_on->write(writer);
        } else {
            block_header_off->write(writer);
        }
        referenceOptions.write(writer);
        writer.write(static_cast<uint8_t>(dataset_type), 4);
        if ((block_header_on != boost::none && block_header_on->getMITFlag()) || block_header_on == boost::none) {
            writer.write(mit_configs.size(), 4);
            for (const auto & c : mit_configs) {
                c.write(writer);
            }
        }
        writer.write(parameters_update_flag, 1);
        writer.write(static_cast<uint8_t>(alphabet_id), 7);
        writer.write(num_U_access_units, 32);
        if (num_U_access_units) {
            u_options->write(writer);
        }
        for (const auto& t : thresholds) {
            writer.write(t != boost::none, 1);
            if (t != boost::none) {
                writer.write(*t, 31);
            }
        }
        writer.flush();
    }

    void addRefSequence(uint8_t _reference_ID, uint16_t _seqID, uint32_t _blocks_num,
                        boost::optional<uint32_t> _threshold) {
        UTILS_DIE_IF(_threshold == boost::none && thresholds.empty(), "First threshold must be supplied");
        referenceOptions.addSeq(_reference_ID, _seqID, _blocks_num);
        thresholds.emplace_back(_threshold);
    }

    void setUAUs(uint32_t _num_U_access_units, UOptions u_opts) {
        UTILS_DIE_IF(!_num_U_access_units, "Resetting num_u_acces_units not supported");
        num_U_access_units = _num_U_access_units;
        u_options = u_opts;
    }

    void addClassConfig(MITClassConfig config) {
        UTILS_DIE_IF(block_header_on != boost::none && !block_header_on->getMITFlag(),
                     "Adding classes without MIT has no effect");
        UTILS_DIE_IF(config.getDescriptorIDs().empty() && block_header_off != boost::none,
                     "Descriptor streams not supplied (block_header_flag)");
        if ((!config.getDescriptorIDs().empty() && block_header_off == boost::none)) {
            config = MITClassConfig(config.getClassID());
        }
        UTILS_DIE_IF(!mit_configs.empty() && mit_configs.back().getClassID() >= config.getClassID(),
                     "Class IDs must be in order.");
        mit_configs.emplace_back(std::move(config));
    }

    void disableBlockHeader(BlockHeaderOffOptions opts) {
        UTILS_DIE_IF(!mit_configs.empty(), "Disabling block header after adding MIT information not supported.");
        block_header_on = boost::none;
        block_header_off = opts;
    }

    void disableMIT() {
        UTILS_DIE_IF(block_header_on == boost::none, "MIT can only be disabled when block headers are activated");
        block_header_on = BlockHeaderOnOptions(false, block_header_on->getCCFlag());
        mit_configs.clear();
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

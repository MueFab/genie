/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_header.h"
#include <limits>
#include <sstream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOnOptions::operator==(const BlockHeaderOnOptions& other) const {
    return mit_flag == other.mit_flag && cc_mode_flag == other.cc_mode_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

BlockHeaderOnOptions::BlockHeaderOnOptions(bool _mit_flag, bool _cc_mode_flag)
    : mit_flag(_mit_flag), cc_mode_flag(_cc_mode_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

BlockHeaderOnOptions::BlockHeaderOnOptions(genie::util::BitReader& reader) {
    mit_flag = reader.read<bool>(1);
    cc_mode_flag = reader.read<bool>(1);
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockHeaderOnOptions::write(genie::util::BitWriter& writer) const {
    writer.write(mit_flag, 1);
    writer.write(cc_mode_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOnOptions::getMITFlag() const { return mit_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOnOptions::getCCFlag() const { return cc_mode_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOffOptions::operator==(const BlockHeaderOffOptions& other) const {
    return ordered_blocks_flag == other.ordered_blocks_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

BlockHeaderOffOptions::BlockHeaderOffOptions(bool _ordered_blocks_flag) : ordered_blocks_flag(_ordered_blocks_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

BlockHeaderOffOptions::BlockHeaderOffOptions(util::BitReader& reader) { ordered_blocks_flag = reader.read<bool>(1); }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOffOptions::getOrderedBlocksFlag() const { return ordered_blocks_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void BlockHeaderOffOptions::write(genie::util::BitWriter& writer) const { writer.write(ordered_blocks_flag, 1); }

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceOptions::operator==(const ReferenceOptions& other) const {
    return reference_ID == other.reference_ID && seq_ID == other.seq_ID && seq_blocks == other.seq_blocks;
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions(genie::util::BitReader& reader) {
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

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceOptions::write(genie::util::BitWriter& writer) const {
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

// ---------------------------------------------------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions() : reference_ID(std::numeric_limits<uint8_t>::max()) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceOptions::addSeq(uint8_t _reference_ID, uint8_t _seq_id, uint16_t blocks) {
    UTILS_DIE_IF(_reference_ID != reference_ID && !seq_ID.empty(), "Unmatching ref id");
    reference_ID = _reference_ID;
    seq_ID.push_back(_seq_id);
    seq_blocks.push_back(blocks);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& ReferenceOptions::getSeqIDs() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& ReferenceOptions::getSeqBlocks() const { return seq_blocks; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceOptions::getReferenceID() const { return reference_ID; }

// ---------------------------------------------------------------------------------------------------------------------

bool MITClassConfig::operator==(const MITClassConfig& other) const {
    return id == other.id && descriptor_ids == other.descriptor_ids;
}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(genie::core::record::ClassType _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(genie::util::BitReader& reader, bool block_header_flag) {
    id = reader.read<genie::core::record::ClassType>(4);
    if (!block_header_flag) {
        auto num_descriptors = reader.read<uint8_t>(5);
        for (size_t i = 0; i < num_descriptors; ++i) {
            descriptor_ids.emplace_back(reader.read<genie::core::GenDesc>(7));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::write(genie::util::BitWriter& writer) const {
    writer.write(static_cast<uint8_t>(id), 4);
    if (!descriptor_ids.empty()) {
        writer.write(descriptor_ids.size(), 5);
        for (const auto& d : descriptor_ids) {
            writer.write(static_cast<uint8_t>(d), 7);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::addDescriptorID(genie::core::GenDesc desc) { descriptor_ids.emplace_back(desc); }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::ClassType MITClassConfig::getClassID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<genie::core::GenDesc>& MITClassConfig::getDescriptorIDs() const { return descriptor_ids; }

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::operator==(const USignature& other) const { return const_length == other.const_length; }

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature() : const_length(boost::none) {}

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature(uint8_t _const_length) : const_length(_const_length) {}

// ---------------------------------------------------------------------------------------------------------------------

USignature::USignature(genie::util::BitReader& reader) {
    bool U_signature_constant_length = reader.read<bool>(1);
    if (U_signature_constant_length) {
        const_length = reader.read<uint8_t>(8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void USignature::write(genie::util::BitWriter& writer) const {
    writer.write(isConstLength(), 1);
    if (isConstLength()) {
        writer.write(getConstLength(), 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool USignature::isConstLength() const { return const_length != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t USignature::getConstLength() const { return *const_length; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::operator==(const UOptions& other) const {
    return reserved1 == other.reserved1 && u_signature == other.u_signature && reserved2 == other.reserved2 &&
           reserved3 == other.reserved3;
}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(uint64_t _reserved1, bool _reserved3)
    : reserved1(_reserved1), u_signature(boost::none), reserved2(boost::none), reserved3(_reserved3) {}

// ---------------------------------------------------------------------------------------------------------------------

UOptions::UOptions(genie::util::BitReader& reader) {
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

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::write(genie::util::BitWriter& writer) const {
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

// ---------------------------------------------------------------------------------------------------------------------

uint64_t UOptions::getReserved1() const { return reserved1; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::getReserved3() const { return reserved3; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::hasReserved2() const { return reserved2 != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t UOptions::getReserved2() const { return *reserved2; }

// ---------------------------------------------------------------------------------------------------------------------

bool UOptions::hasSignature() const { return u_signature != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const USignature& UOptions::getSignature() const { return *u_signature; }

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::addSignature(USignature s) { u_signature = s; }

// ---------------------------------------------------------------------------------------------------------------------

void UOptions::addReserved2(uint8_t r) { reserved2 = r; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetHeader&>(info);
    return group_ID == other.group_ID && ID == other.ID && version == other.version &&
           multiple_alignment_flag == other.multiple_alignment_flag &&
           byte_offset_size_flag == other.byte_offset_size_flag &&
           non_overlapping_AU_range_flag == other.non_overlapping_AU_range_flag &&
           pos_40_bits_flag == other.pos_40_bits_flag && block_header_on == other.block_header_on &&
           block_header_off == other.block_header_off && referenceOptions == other.referenceOptions &&
           dataset_type == other.dataset_type && mit_configs == other.mit_configs &&
           parameters_update_flag == other.parameters_update_flag && alphabet_id == other.alphabet_id &&
           num_U_access_units == other.num_U_access_units && u_options == other.u_options &&
           thresholds == other.thresholds;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetHeader::getDatasetGroupID() const { return group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetHeader::getDatasetID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::MPEGMinorVersion DatasetHeader::getVersion() const { return version; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetHeader::getByteOffsetSize() const { return byte_offset_size_flag ? 64 : 32; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getNonOverlappingAURangeFlag() const { return non_overlapping_AU_range_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetHeader::getPosBits() const { return pos_40_bits_flag ? 40 : 32; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isBlockHeaderEnabled() const { return block_header_on != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isMITEnabled() const { return block_header_off != boost::none || block_header_on->getMITFlag(); }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isCCModeEnabled() const { return block_header_on != boost::none && block_header_on->getCCFlag(); }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isOrderedBlockMode() const {
    return block_header_off != boost::none && block_header_off->getOrderedBlocksFlag();
}

// ---------------------------------------------------------------------------------------------------------------------

const ReferenceOptions& DatasetHeader::getReferenceOptions() const { return referenceOptions; }

// ---------------------------------------------------------------------------------------------------------------------

core::parameter::DataUnit::DatasetType DatasetHeader::getDatasetType() const { return dataset_type; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<MITClassConfig>& DatasetHeader::getMITConfigs() const { return mit_configs; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getParameterUpdateFlag() const { return parameters_update_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlphabetID DatasetHeader::getAlphabetID() const { return alphabet_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t DatasetHeader::getNumUAccessUnits() const { return num_U_access_units; }

// ---------------------------------------------------------------------------------------------------------------------

const UOptions& DatasetHeader::getUOptions() const { return *u_options; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<boost::optional<uint32_t>>& DatasetHeader::getRefSeqThresholds() const { return thresholds; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetHeader::getKey() const {
    static const std::string key = "dthd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetHeader::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader()
    : DatasetHeader(0, 0, genie::core::MPEGMinorVersion::V2000, false, false, false, false,
                    core::parameter::DataUnit::DatasetType::ALIGNED, false, core::AlphabetID::ACGTN) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader(uint8_t _dataset_group_id, uint16_t _dataset_id, genie::core::MPEGMinorVersion _version,
                             bool _multiple_alignments_flags, bool _byte_offset_size_flags,
                             bool _non_overlapping_AU_range_flag, bool _pos_40_bits_flag,
                             core::parameter::DataUnit::DatasetType _dataset_type, bool _parameters_update_flag,
                             core::AlphabetID _alphabet_id)
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

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader(genie::util::BitReader& reader) {
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

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::write(genie::util::BitWriter& writer) const {
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
        for (const auto& c : mit_configs) {
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

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::addRefSequence(uint8_t _reference_ID, uint16_t _seqID, uint32_t _blocks_num,
                                   boost::optional<uint32_t> _threshold) {
    UTILS_DIE_IF(_threshold == boost::none && thresholds.empty(), "First threshold must be supplied");
    referenceOptions.addSeq(_reference_ID, _seqID, _blocks_num);
    thresholds.emplace_back(_threshold);
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::setUAUs(uint32_t _num_U_access_units, UOptions u_opts) {
    UTILS_DIE_IF(!_num_U_access_units, "Resetting num_u_acces_units not supported");
    num_U_access_units = _num_U_access_units;
    u_options = u_opts;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::addClassConfig(MITClassConfig config) {
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

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::disableBlockHeader(BlockHeaderOffOptions opts) {
    UTILS_DIE_IF(!mit_configs.empty(), "Disabling block header after adding MIT information not supported.");
    block_header_on = boost::none;
    block_header_off = opts;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::disableMIT() {
    UTILS_DIE_IF(block_header_on == boost::none, "MIT can only be disabled when block headers are activated");
    block_header_on = BlockHeaderOnOptions(false, block_header_on->getCCFlag());
    mit_configs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

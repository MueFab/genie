/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header.h"
#include <limits>
#include <sstream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

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

bool DatasetHeader::isBlockHeaderEnabled() const { return block_header_on != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isMITEnabled() const { return block_header_off != std::nullopt || block_header_on->getMITFlag(); }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isCCModeEnabled() const { return block_header_on != std::nullopt && block_header_on->getCCFlag(); }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::isOrderedBlockMode() const {
    return block_header_off != std::nullopt && block_header_off->getOrderedBlocksFlag();
}

// ---------------------------------------------------------------------------------------------------------------------

const dataset_header::ReferenceOptions& DatasetHeader::getReferenceOptions() const { return referenceOptions; }

// ---------------------------------------------------------------------------------------------------------------------

core::parameter::DataUnit::DatasetType DatasetHeader::getDatasetType() const { return dataset_type; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<dataset_header::MITClassConfig>& DatasetHeader::getMITConfigs() const { return mit_configs; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetHeader::getParameterUpdateFlag() const { return parameters_update_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlphabetID DatasetHeader::getAlphabetID() const { return alphabet_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t DatasetHeader::getNumUAccessUnits() const { return num_U_access_units; }

// ---------------------------------------------------------------------------------------------------------------------

const dataset_header::UOptions& DatasetHeader::getUOptions() const { return *u_options; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::optional<uint32_t>>& DatasetHeader::getRefSeqThresholds() const { return thresholds; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetHeader::getKey() const {
    static const std::string key = "dthd";
    return key;
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
    block_header_on = dataset_header::BlockHeaderOnOptions{false, false};
    num_U_access_units = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader::DatasetHeader(genie::util::BitReader& reader) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    group_ID = reader.readAlignedInt<uint8_t>();
    ID = reader.readAlignedInt<uint16_t>();
    std::string versionString(4, '\0');
    reader.readAlignedBytes(versionString.data(), versionString.length());
    version = core::getMPEGVersion(versionString);
    UTILS_DIE_IF(version == core::MPEGMinorVersion::UNKNOWN, "Unknown MPEG version");

    multiple_alignment_flag = reader.read<bool>(1);
    byte_offset_size_flag = reader.read<bool>(1);
    non_overlapping_AU_range_flag = reader.read<bool>(1);
    pos_40_bits_flag = reader.read<bool>(1);
    bool block_header_flag = reader.read<bool>(1);
    if (block_header_flag) {
        block_header_on = dataset_header::BlockHeaderOnOptions(reader);
    } else {
        block_header_off = dataset_header::BlockHeaderOffOptions(reader);
    }
    referenceOptions = dataset_header::ReferenceOptions(reader);
    dataset_type = reader.read<genie::core::parameter::DataUnit::DatasetType>(4);
    if ((block_header_on != std::nullopt && block_header_on->getMITFlag()) || block_header_on == std::nullopt) {
        auto num_classes = reader.read<uint8_t>(4);
        for (size_t i = 0; i < num_classes; ++i) {
            mit_configs.emplace_back(reader, block_header_flag);
        }
    }
    parameters_update_flag = reader.read<bool>(1);
    alphabet_id = reader.read<genie::core::AlphabetID>(7);
    num_U_access_units = reader.read<uint32_t>(32);
    if (num_U_access_units) {
        u_options = dataset_header::UOptions(reader);
    }
    for (size_t i = 0; i < referenceOptions.getSeqIDs().size(); ++i) {
        bool flag = reader.read<bool>(1);
        UTILS_DIE_IF(flag == false && i == 0, "First ref must provide treshold");
        if (flag) {
            thresholds.emplace_back(reader.read<uint32_t>(31));
        } else {
            thresholds.emplace_back(std::nullopt);
        }
    }
    reader.flushHeldBits();
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::box_write(genie::util::BitWriter& writer) const {
    writer.writeAlignedInt<uint8_t>(group_ID);
    writer.writeAlignedInt<uint16_t>(ID);
    const auto& v_string = genie::core::getMPEGVersionString(version);
    writer.writeAlignedBytes(v_string.data(), v_string.length());

    writer.writeBits(multiple_alignment_flag, 1);
    writer.writeBits(byte_offset_size_flag, 1);
    writer.writeBits(non_overlapping_AU_range_flag, 1);
    writer.writeBits(pos_40_bits_flag, 1);
    writer.writeBits(block_header_on != std::nullopt, 1);
    if (block_header_on != std::nullopt) {
        block_header_on->write(writer);
    } else {
        block_header_off->write(writer);
    }
    referenceOptions.write(writer);
    writer.writeBits(static_cast<uint8_t>(dataset_type), 4);
    if ((block_header_on != std::nullopt && block_header_on->getMITFlag()) || block_header_on == std::nullopt) {
        writer.writeBits(mit_configs.size(), 4);
        for (const auto& c : mit_configs) {
            c.write(writer);
        }
    }
    writer.writeBits(parameters_update_flag, 1);
    writer.writeBits(static_cast<uint8_t>(alphabet_id), 7);
    writer.writeBits(num_U_access_units, 32);
    if (num_U_access_units) {
        u_options->write(writer);
    }
    for (const auto& t : thresholds) {
        writer.writeBits(t != std::nullopt, 1);
        if (t != std::nullopt) {
            writer.writeBits(*t, 31);
        }
    }
    writer.flushBits();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::addRefSequence(uint8_t _reference_ID, uint16_t _seqID, uint32_t _blocks_num,
                                   std::optional<uint32_t> _threshold) {
    UTILS_DIE_IF(_threshold == std::nullopt && thresholds.empty(), "First threshold must be supplied");
    referenceOptions.addSeq(_reference_ID, _seqID, _blocks_num);
    thresholds.emplace_back(_threshold);
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::setUAUs(uint32_t _num_U_access_units, dataset_header::UOptions u_opts) {
    UTILS_DIE_IF(!_num_U_access_units, "Resetting num_u_acces_units not supported");
    num_U_access_units = _num_U_access_units;
    u_options = u_opts;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::addClassConfig(dataset_header::MITClassConfig config) {
    UTILS_DIE_IF(block_header_on != std::nullopt && !block_header_on->getMITFlag(),
                 "Adding classes without MIT has no effect");
    UTILS_DIE_IF(config.getDescriptorIDs().empty() && block_header_off != std::nullopt,
                 "Descriptor streams not supplied (block_header_flag)");
    if ((!config.getDescriptorIDs().empty() && block_header_off == std::nullopt)) {
        config = dataset_header::MITClassConfig(config.getClassID());
    }
    UTILS_DIE_IF(!mit_configs.empty() && mit_configs.back().getClassID() >= config.getClassID(),
                 "Class IDs must be in order.");
    mit_configs.emplace_back(std::move(config));
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::disableBlockHeader(dataset_header::BlockHeaderOffOptions opts) {
    UTILS_DIE_IF(!mit_configs.empty(), "Disabling block header after adding MIT information not supported.");
    block_header_on = std::nullopt;
    block_header_off = opts;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::disableMIT() {
    UTILS_DIE_IF(block_header_on == std::nullopt, "MIT can only be disabled when block headers are activated");
    block_header_on = dataset_header::BlockHeaderOnOptions(false, block_header_on->getCCFlag());
    mit_configs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::patchRefID(uint8_t _old, uint8_t _new) { referenceOptions.patchRefID(_old, _new); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::patchID(uint8_t _groupID, uint16_t _setID) {
    group_ID = _groupID;
    ID = _setID;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::meta::BlockHeader> DatasetHeader::decapsulate() {
    if (block_header_on != std::nullopt) {
        return std::make_unique<genie::core::meta::blockheader::Enabled>(block_header_on->getMITFlag(),
                                                                                 block_header_on->getCCFlag());
    } else {
        return std::make_unique<genie::core::meta::blockheader::Disabled>(
            block_header_off->getOrderedBlocksFlag());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetHeader::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Dataset Header");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

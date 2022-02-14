/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset.h"
#include <iostream>
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(util::BitReader& reader, core::MPEGMinorVersion _version) : version(_version) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    auto end_pos = start_pos + static_cast<int64_t>(length);
    std::string tmp(4, '\0');
    reader.readBypass(tmp);
    UTILS_DIE_IF(tmp != "dthd", "Dataset without header");
    header = DatasetHeader(reader);
    while (reader.getPos() != end_pos) {
        UTILS_DIE_IF(reader.getPos() > end_pos, "Read too far");
        UTILS_DIE_IF(!reader.isGood(), "Reader died");
        read_box(reader, false);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const DatasetHeader& Dataset::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::box_write(genie::util::BitWriter& bitWriter) const {
    header.write(bitWriter);
    if (metadata) {
        metadata->write(bitWriter);
    }
    if (protection) {
        protection->write(bitWriter);
    }
    for (const auto& p : parameterSets) {
        p.write(bitWriter);
    }
    if (master_index_table) {
        master_index_table->write(bitWriter);
    }
    for (const auto& p : access_units) {
        p.write(bitWriter);
    }
    for (const auto& p : descriptor_streams) {
        p.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Dataset::getKey() const {
    static const std::string key = "dtcn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Dataset::hasMetadata() const { return metadata != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

bool Dataset::hasProtection() const { return metadata != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

DatasetMetadata& Dataset::getMetadata() { return *metadata; }

// ---------------------------------------------------------------------------------------------------------------------

DatasetProtection& Dataset::getProtection() { return *protection; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<AccessUnit>& Dataset::getAccessUnits() { return access_units; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DescriptorStream>& Dataset::getDescriptorStreams() { return descriptor_streams; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DatasetParameterSet>& Dataset::getParameterSets() { return parameterSets; }

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(format::mgb::MgbFile& file, core::meta::Dataset& meta, core::MPEGMinorVersion _version,
                 const std::vector<uint8_t>& param_ids)
    : version(_version) {
    bool mitFlag = false;
    bool cc_mode = false;
    bool ordered_blocks = false;
    bool headerON = meta.getHeader().getType() == core::meta::BlockHeader::HeaderType::ENABLED;
    if (headerON) {
        cc_mode = dynamic_cast<const core::meta::blockheader::Enabled&>(meta.getHeader()).getCCFlag();
        mitFlag = dynamic_cast<const core::meta::blockheader::Enabled&>(meta.getHeader()).getMITFlag();
    } else {
        ordered_blocks = dynamic_cast<const core::meta::blockheader::Disabled&>(meta.getHeader()).getOrderedFlag();
    }

    if (!headerON) {
        if (ordered_blocks) {
            file.sort_by_position();
        }
        for (size_t c = 0; c < size_t(core::record::ClassType::COUNT); ++c) {
            for (size_t d = 0; d < size_t(core::GenDesc::COUNT); ++d) {
                auto blocks = file.extractDescriptor(core::record::ClassType(c), core::GenDesc(d), param_ids);
                auto desc = DescriptorStream(core::GenDesc(d), core::record::ClassType(c), blocks);
                if (!desc.isEmpty()) {
                    descriptor_streams.emplace_back(std::move(desc));
                    for (auto& b : meta.getDSs()) {
                        if (core::GenDesc(b.getID()) == descriptor_streams.back().getHeader().getDescriptorID()) {
                            descriptor_streams.back().setProtection(
                                DescriptorStreamProtection(std::move(b.getProtection())));
                        }
                    }
                }
            }
        }
        file.clearAUBlocks(param_ids);
    } else {
        if (cc_mode) {
            file.sort_by_class();
        } else {
            file.sort_by_position();
        }
    }

    auto params_p2 = file.extractParameters(param_ids);
    for (auto& p : params_p2) {
        parameterSets.emplace_back(static_cast<uint8_t>(0), static_cast<uint16_t>(0), p->getID(), p->getParentID(),
                                   std::move(p->getEncodingSet()), version);
    }

    auto access_units_p2 = file.extractAUs(param_ids);

    for (auto& a : access_units_p2) {
        access_units.emplace_back(std::move(*a), mitFlag, version);
        for (auto& b : meta.getAUs()) {
            if (b.getID() == access_units.back().getHeader().getHeader().getID()) {
                access_units.back().setInformation(AUInformation(0, 0, std::move(b.getInformation()), _version));
                access_units.back().setProtection(AUProtection(0, 0, std::move(b.getProtection()), _version));
            }
        }
    }

    header = DatasetHeader(0, 0, version, parameterSets.front().getEncodingSet().hasMultipleAlignments(), true, false,
                           parameterSets.front().getEncodingSet().getPosSize() == 40,
                           parameterSets.front().getEncodingSet().getDatasetType(), false,
                           parameterSets.front().getEncodingSet().getAlphabetID());

    if (!meta.getInformation().empty()) {
        metadata = DatasetMetadata(0, 0, std::move(meta.getInformation()), _version);
    }

    if (!meta.getProtection().empty()) {
        protection = DatasetProtection(0, 0, std::move(meta.getProtection()), _version);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::patchID(uint8_t groupID, uint16_t setID) {
    header.patchID(groupID, setID);
    if (metadata != boost::none) {
        metadata->patchID(groupID, setID);
    }
    if (protection != boost::none) {
        protection->patchID(groupID, setID);
    }
    for (auto& ps : parameterSets) {
        ps.patchID(groupID, setID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::patchRefID(uint8_t _old, uint8_t _new) { header.patchRefID(_old, _new); }

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader& Dataset::getHeader() { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::read_box(util::BitReader& reader, bool in_offset) {
    std::string tmp_str(4, '\0');
    reader.readBypass(tmp_str);
    if (tmp_str == "dtmd") {
        UTILS_DIE_IF(metadata != boost::none, "Metadata already present");
        UTILS_DIE_IF(protection != boost::none, "Metadata must be before protection");
        UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
        UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
        UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
        UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
        metadata = DatasetMetadata(reader, version);
    } else if (tmp_str == "dtpr") {
        UTILS_DIE_IF(protection != boost::none, "Protection already present");
        UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
        UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
        UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
        UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
        protection = DatasetProtection(reader, version);
    } else if (tmp_str == "pars") {
        UTILS_DIE_IF(in_offset, "Offset not permitted");
        UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
        UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
        UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
        parameterSets.emplace_back(reader, version, header.getParameterUpdateFlag());
        encoding_sets.emplace(
            std::make_pair(size_t(parameterSets.back().getParameterSetID()), parameterSets.back().getEncodingSet()));
    } else if (tmp_str == "mitb") {
        UTILS_DIE_IF(in_offset, "Offset not permitted");
        UTILS_DIE_IF(master_index_table != boost::none, "MIT already present");
        UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
        UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
        master_index_table = MasterIndexTable(reader, header);
    } else if (tmp_str == "aucn") {
        UTILS_DIE_IF(in_offset, "Offset not permitted");
        UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
        access_units.emplace_back(reader, encoding_sets, header.isMITEnabled(), header.isBlockHeaderEnabled(), version);
    } else if (tmp_str == "dscn") {
        UTILS_DIE_IF(in_offset, "Offset not permitted");
        UTILS_DIE_IF(master_index_table == boost::none, "descriptor streams without MIT not allowed");
        UTILS_DIE_IF(header.isBlockHeaderEnabled(), "descriptor streams only allowed without block headers");
        descriptor_streams.emplace_back(reader, *master_index_table, header.getMITConfigs());
    } else if (tmp_str == "offs") {
        UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
        reader.readBypass(tmp_str);
        uint64_t offset = reader.readBypassBE<uint64_t>();
        if (offset == ~static_cast<uint64_t>(0)) {
            return;
        }
        auto pos_save = reader.getPos();
        reader.setPos(offset);
        read_box(reader, true);
        reader.setPos(pos_save);
    } else {
        UTILS_DIE("Unknown box");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Dataset");
    header.print_debug(output, depth + 1, max_depth);
    if (metadata) {
        metadata->print_debug(output, depth + 1, max_depth);
    }
    if (protection) {
        metadata->print_debug(output, depth + 1, max_depth);
    }
    for (const auto& r : parameterSets) {
        r.print_debug(output, depth + 1, max_depth);
    }
    if (master_index_table) {
        master_index_table->print_debug(output, depth + 1, max_depth);
    }
#ifdef GENIE_DEBUG_PRINT_NODETAIL
    if (!access_units.empty()) {
        print_offset(output, depth + 1, max_depth, "* " + std::to_string(access_units.size()) + " access units");
    }
    if (!descriptor_streams.empty()) {
        print_offset(output, depth + 1, max_depth,
                     "* " + std::to_string(descriptor_streams.size()) + " descriptor streams");
    }
#else
    for (const auto& r : access_units) {
        r.print_debug(output, depth + 1, max_depth);
    }
    for (const auto& r : descriptor_streams) {
        r.print_debug(output, depth + 1, max_depth);
    }
#endif
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

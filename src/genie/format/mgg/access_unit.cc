/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/access_unit.h"
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const AccessUnit&>(info);
    return header == other.header && au_information == other.au_information && au_protection == other.au_protection &&
           blocks == other.blocks;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(AccessUnitHeader h, core::MPEGMinorVersion _verison) : header(std::move(h)), version(_verison) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(util::BitReader& reader, const std::map<size_t, core::parameter::EncodingSet>& parameterSets,
                       bool mit, bool block_header, core::MPEGMinorVersion _version)
    : version(_version) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    std::string tmp(4, '\0');
    reader.readAlignedBytes(tmp.data(), tmp.length());
    UTILS_DIE_IF(tmp != "auhd", "Access unit without header");
    header = AccessUnitHeader(reader, parameterSets, mit);
    do {
        auto tmp_pos = reader.getStreamPosition();
        std::string tmp_str(4, '\0');
        reader.readAlignedBytes(tmp_str.data(), tmp_str.length());
        if (tmp_str == "auin") {
            UTILS_DIE_IF(au_information != std::nullopt, "AU-Inf already present");
            UTILS_DIE_IF(au_protection != std::nullopt, "AU-Inf must be before AU-PR");
            au_information = AUInformation(reader, version);
        } else if (tmp_str == "aupr") {
            UTILS_DIE_IF(au_protection != std::nullopt, "AU-Pr already present");
            au_protection = AUProtection(reader, version);
        } else {
            reader.setStreamPosition(tmp_pos);
            break;
        }
    } while (true);
    if (block_header) {
        for (size_t i = 0; i < header.getHeader().getNumBlocks(); ++i) {
            blocks.emplace_back(reader);
        }
    }
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addBlock(Block b) { blocks.emplace_back(std::move(b)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Block>& AccessUnit::getBlocks() const { return blocks; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitHeader& AccessUnit::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::hasInformation() const { return au_information != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::hasProtection() const { return au_protection != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const AUInformation& AccessUnit::getInformation() const { return *au_information; }

// ---------------------------------------------------------------------------------------------------------------------

const AUProtection& AccessUnit::getProtection() const { return *au_protection; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setInformation(AUInformation au) { au_information = std::move(au); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setProtection(AUProtection au) { au_protection = std::move(au); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::box_write(genie::util::BitWriter& bitWriter) const {
    header.write(bitWriter);
    if (au_information != std::nullopt) {
        au_information->write(bitWriter);
    }
    if (au_protection != std::nullopt) {
        au_protection->write(bitWriter);
    }
    for (const auto& b : blocks) {
        b.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AccessUnit::getKey() const {
    static const std::string key = "aucn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Access Unit");
    header.print_debug(output, depth + 1, max_depth);
    if (au_information) {
        au_information->print_debug(output, depth + 1, max_depth);
    }
    if (au_protection) {
        au_protection->print_debug(output, depth + 1, max_depth);
    }
    for (const auto& r : blocks) {
        r.print_debug(output, depth + 1, max_depth);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AUInformation& AccessUnit::getInformation() { return *au_information; }

// ---------------------------------------------------------------------------------------------------------------------

AUProtection& AccessUnit::getProtection() { return *au_protection; }

// ---------------------------------------------------------------------------------------------------------------------

format::mgb::AccessUnit AccessUnit::decapsulate() {
    std::vector<format::mgb::Block> newBlocks;
    newBlocks.reserve(blocks.size());
    for (auto& b : blocks) {
        newBlocks.emplace_back(b.decapsulate());
    }
    return {std::move(header.getHeader()), std::move(newBlocks)};
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(format::mgb::AccessUnit au, bool mit, core::MPEGMinorVersion _version)
    : header(std::move(au.getHeader()), mit), version(_version) {
    for (auto& b : au.getBlocks()) {
        blocks.emplace_back(std::move(b));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

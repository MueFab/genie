/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/access_unit.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
    reader.readBypassBE<uint64_t>();
    std::string tmp(4, '\0');
    reader.readBypass(tmp);
    UTILS_DIE_IF(tmp != "auhd", "Access unit without header");
    header = AccessUnitHeader(reader, parameterSets, mit);
    do {
        auto tmp_pos = reader.getPos();
        std::string tmp_str(4, '\0');
        reader.readBypass(tmp_str);
        if (tmp_str == "auin") {
            UTILS_DIE_IF(au_information != boost::none, "AU-Inf already present");
            UTILS_DIE_IF(au_protection != boost::none, "AU-Inf must be before AU-PR");
            au_information = AUInformation(reader, version);
        } else if (tmp_str == "aupr") {
            UTILS_DIE_IF(au_protection != boost::none, "AU-Pr already present");
            au_protection = AUProtection(reader, version);
        } else {
            reader.setPos(tmp_pos);
            break;
        }
    } while (true);
    if (block_header) {
        for (size_t i = 0; i < header.getHeader().getNumBlocks(); ++i) {
            blocks.emplace_back(reader);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addBlock(Block b) { blocks.emplace_back(std::move(b)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Block>& AccessUnit::getBlocks() const { return blocks; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitHeader& AccessUnit::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::hasInformation() const { return au_information != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::hasProtection() const { return au_protection != boost::none; }

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
    if (au_information != boost::none) {
        au_information->write(bitWriter);
    }
    if (au_protection != boost::none) {
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

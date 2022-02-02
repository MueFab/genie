/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset.h"
#include <iostream>
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(util::BitReader& reader, core::MPEGMinorVersion _version) : version(_version) {
    auto start_pos = reader.getPos();
    auto length = reader.readBypassBE<uint64_t>();
    auto end_pos = start_pos + static_cast<int64_t>(length) - 4;
    std::string tmp(4, '\0');
    reader.readBypass(tmp);
    UTILS_DIE_IF(tmp != "dthd", "Dataset without header");
    header = DatasetHeader(reader);
    while (reader.getPos() < end_pos) {
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

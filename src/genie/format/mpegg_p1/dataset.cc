/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(util::BitReader& reader, core::MPEGMinorVersion _version) : version(_version) {
    auto start_pos = reader.getPos();
    auto length = reader.readBypassBE<uint64_t>();
    auto end_pos = start_pos + static_cast<int64_t>(length) - 4;
    header = DatasetHeader(reader);
    while (reader.getPos() < end_pos) {
        reader.getPos();
        std::string tmp_str(4, '\0');
        reader.readBypass(tmp_str);
        if (tmp_str == "dtmd") {
            UTILS_DIE_IF(metadata != boost::none, "Metadata already present");
            UTILS_DIE_IF(protection != boost::none, "Metadata must be before protection");
            UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
            UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
            UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
            UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
            metadata = DatasetMetadata(reader);
        } else if (tmp_str == "dtpr") {
            UTILS_DIE_IF(protection != boost::none, "Protection already present");
            UTILS_DIE_IF(!parameterSets.empty(), "Metadata must be before parametersets");
            UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
            UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
            UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
            protection = DatasetProtection(reader);
        } else if (tmp_str == "pars") {
            UTILS_DIE_IF(master_index_table != boost::none, "Metadata must be before MIT");
            UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
            UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
            parameterSets.emplace_back(reader, version, header.getParameterUpdateFlag());
            encoding_sets.emplace(std::make_pair(size_t(parameterSets.back().getParameterSetID()),
                                                 parameterSets.back().getEncodingSet()));
        } else if (tmp_str == "mitb") {
            UTILS_DIE_IF(master_index_table != boost::none, "MIT already present");
            UTILS_DIE_IF(!access_units.empty(), "Metadata must be before Access Units");
            UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
            master_index_table = MasterIndexTable(reader, header);
        } else if (tmp_str == "aucn") {
            UTILS_DIE_IF(!descriptor_streams.empty(), "Metadata must be before Descriptor streams");
            access_units.emplace_back(reader, encoding_sets, header.isMITEnabled());
        } else if (tmp_str == "dscn") {
            UTILS_DIE_IF(master_index_table == boost::none, "descriptor streams without MIT not allowed");
            UTILS_DIE_IF(header.isBlockHeaderEnabled(), "descriptor streams only allowed without block headers");
            // TODO: How to calculate offsets
            //     descriptor_streams.emplace_back(reader, master_index_table.get);
        } else {
            UTILS_DIE("Unknown box");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const DatasetHeader& Dataset::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::write(genie::util::BitWriter& bitWriter) const { GenInfo::write(bitWriter); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Dataset::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
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

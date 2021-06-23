/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/master_index_table/master_index_table.h"
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable() : ac_info(), u_ac_info(), datasetHeader() {}

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(std::vector<mpegg_p1::MITAccessUnitInfo>&& _ac_info,
                                   std::vector<mpegg_p1::MITUAccessUnitInfo>&& _u_ac_info, DatasetHeader* header)
    : ac_info(std::move(_ac_info)), u_ac_info(std::move(_u_ac_info)), datasetHeader(header) {}

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(util::BitReader& reader, size_t length, DatasetHeader* _header)
    : datasetHeader(_header) {
    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "mitb", "MasterIndexTable is not Found");

    size_t start_pos = reader.getPos();

    /// Class MITAccessUnitInfo()
    for (auto seq = 0; seq < datasetHeader->getSeqInfo().getSeqCount(); seq++) {
        for (auto ci = 0; ci < datasetHeader->getBlockHeader().getNumClasses(); ci++) {
            if (datasetHeader->getBlockHeader().getClassInfos()[ci].getClid() != core::record::ClassType::CLASS_U) {
                for (auto au_id = 0; au_id < datasetHeader->getSeqInfo().getSeqBlocks()[seq]; au_id++) {
                    ac_info.emplace_back(reader);
                }
            }
        }
    }

    /// Class MITUAccessUnitInfo()
    for (uint32_t uau_id = 0; uau_id < datasetHeader->getNumUAccessUnits(); uau_id++) {
        u_ac_info.emplace_back(reader);
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid MasterIndexTable length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MasterIndexTable::getLength() const {
    // length is first calculated in bits then converted in bytes
    /// Key c(4) Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8;  // gen_info
    return bitlen / 8;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

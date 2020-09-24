/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/runtime-exception.h>

#include "master_index_table.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MasterIndexTable::MasterIndexTable() = default;

uint64_t MasterIndexTable::getLength() const {
    // TODO (Yeremia): Implement this!
    UTILS_DIE("Not yet implemented!");
    return 0;
}

void MasterIndexTable::write(util::BitWriter& bit_writer) const {
    // TODO (Yeremia): Implement this!
    UTILS_DIE("Not yet implemented!");
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
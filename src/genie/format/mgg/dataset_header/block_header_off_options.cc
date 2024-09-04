/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header/block_header_off_options.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

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

void BlockHeaderOffOptions::write(genie::util::BitWriter& writer) const { writer.writeBits(ordered_blocks_flag, 1); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

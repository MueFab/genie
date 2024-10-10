/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/block_header_on_options.h"

namespace genie::format::mgg::dataset_header {

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
    writer.writeBits(mit_flag, 1);
    writer.writeBits(cc_mode_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOnOptions::getMITFlag() const { return mit_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeaderOnOptions::getCCFlag() const { return cc_mode_flag; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

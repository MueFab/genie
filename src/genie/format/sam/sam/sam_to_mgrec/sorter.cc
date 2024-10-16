/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/sam/sam_to_mgrec/sorter.h"
#include <iostream>
#include <string>
#include <utility>
#include "apps/genie/transcode-sam/utils.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

SubfileReader::SubfileReader(const std::string& fpath)
    : reader(fpath, std::ios::binary), bitreader(reader), rec(), path(fpath) {
    if (good()) {
        rec = genie::core::record::Record(bitreader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SubfileReader::~SubfileReader() {
    bitreader.flushHeldBits();
    reader.close();
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::Record SubfileReader::moveRecord() {
    UTILS_DIE_IF(!rec, "No record available.");
    auto ret = std::move(rec.value());
    if (good()) {
        rec = genie::core::record::Record(bitreader);
    } else {
        rec.reset();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<genie::core::record::Record>& SubfileReader::getRecord() const { return rec; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubfileReader::good() { return reader.good() && reader.peek() != EOF; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

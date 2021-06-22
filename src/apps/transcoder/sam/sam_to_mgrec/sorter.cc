/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/transcoder/sam/sam_to_mgrec/sorter.h"

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/transcoder/utils.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

SubfileReader::SubfileReader(const std::string& fpath)
    : curr_mgrec_pos(0), reader(fpath, std::ios::binary), bitreader(reader), rec() {}

// ---------------------------------------------------------------------------------------------------------------------

SubfileReader::~SubfileReader() { close(); }

// ---------------------------------------------------------------------------------------------------------------------

bool SubfileReader::readRecord() {
    if (good()) {
        rec = genie::core::record::Record(bitreader);

        if (rec.getAlignments().empty()) {
            curr_mgrec_pos = getMinPos(rec);
        }

        return true;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::Record&& SubfileReader::moveRecord() { return std::move(rec); }

// ---------------------------------------------------------------------------------------------------------------------

const genie::core::record::Record& SubfileReader::getRecord() const { return rec; }

// ---------------------------------------------------------------------------------------------------------------------

void SubfileReader::writeRecord(genie::util::BitWriter& bitwriter) {
    rec.write(bitwriter);
    bitwriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t SubfileReader::getPos() const { return curr_mgrec_pos; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubfileReader::good() { return reader.good() && reader.peek() != EOF; }

// ---------------------------------------------------------------------------------------------------------------------

void SubfileReader::close() {
    bitreader.flush();
    reader.close();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

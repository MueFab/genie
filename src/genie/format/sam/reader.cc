/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream) : stream(_stream), header(stream), rec_saved(false) {}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void Reader::read(size_t num, std::vector<Record>& vec, genie::core::stats::SamStats *stats) {
    std::string string;
    vec.clear();
    if (rec_saved) {
        rec_saved = false;
        vec.emplace_back(std::move(save));
    }
    while (num && std::getline(stream, string)) {
        Record rec(string, stats);
        vec.emplace_back(rec);
        if (vec.front().getRname() != vec.back().getRname()) {
            save = std::move(vec.back());
            vec.pop_back();
            rec_saved = true;
            return;
        }
        num--;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isEnd() { return stream.eof(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

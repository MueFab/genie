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

void Reader::read(size_t num, std::vector<Record>& vec, core::stats::PerfStats& stats) {
    std::string string;
    vec.clear();
    if (rec_saved) {
        rec_saved = false;
        vec.emplace_back(std::move(save));
    }
    Record::Stats rstat;
    while (num && std::getline(stream, string)) {
        Record::Stats lstat;
        Record rec(string, lstat);
        rstat.add(lstat);
        vec.emplace_back(rec);
        if (vec.front().getRname() != vec.back().getRname()) {
            save = std::move(vec.back());
            vec.pop_back();
            rec_saved = true;
            return;
        }
        num--;
    }

    stats.addInteger("size-sam-qname", rstat.qname);
    stats.addInteger("size-sam-flag", rstat.flag);
    stats.addInteger("size-sam-rname", rstat.rname);
    stats.addInteger("size-sam-pos", rstat.pos);
    stats.addInteger("size-sam-mapq", rstat.mapq);
    stats.addInteger("size-sam-cigar", rstat.cigar);
    stats.addInteger("size-sam-rnext", rstat.rnext);
    stats.addInteger("size-sam-pnext", rstat.pnext);
    stats.addInteger("size-sam-tlen", rstat.tlen);
    stats.addInteger("size-sam-seq", rstat.seq);
    stats.addInteger("size-sam-qual", rstat.qual);
    stats.addInteger("size-sam-opt", rstat.opt);
    stats.addInteger("size-sam-total", rstat.total());
}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isEnd() { return stream.eof(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

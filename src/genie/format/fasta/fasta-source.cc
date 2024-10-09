/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/fasta/fasta-source.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "genie/util/ordered-section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

// ---------------------------------------------------------------------------------------------------------------------

FastaSource::FastaSource(std::ostream* _outfile, core::ReferenceManager* _refMgr)
    : outfile(_outfile), refMgr(_refMgr), line_length(0) {
    auto seqs = refMgr->getSequences();
    size_t pos = 0;
    for (const auto& s : seqs) {
        pos += (refMgr->getLength(s) - 1) / genie::core::ReferenceManager::getChunkSize() + 1;
        accu_lengths[s] = pos;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool FastaSource::pump(uint64_t& id, std::mutex& lock) {
    util::Section loc_id = {0, 1, false};
    {
        std::lock_guard<std::mutex> guard(lock);
        loc_id.start = id;
        id++;
    }

    if (loc_id.start >= accu_lengths.rbegin()->second) {
        return false;
    }
    std::string seq;
    size_t pos_old = 0;
    size_t pos = 0;
    for (const auto& s : accu_lengths) {
        if (loc_id.start < s.second) {
            pos = loc_id.start - pos_old;
            seq = s.first;
            break;
        }
        pos_old = s.second;
    }

    auto string = refMgr->loadAt(seq, pos * genie::core::ReferenceManager::getChunkSize());
    size_t actual_length = loc_id.start == (accu_lengths[seq] - 1)
                               ? refMgr->getLength(seq) % genie::core::ReferenceManager::getChunkSize()
                               : string->length();

    std::cerr << "Decompressing " << seq << " [" << pos * genie::core::ReferenceManager::getChunkSize() << ", "
              << pos * genie::core::ReferenceManager::getChunkSize() + actual_length << "]" << std::endl;

    util::OrderedSection outSec(&outlock, loc_id);
    if (pos == 0) {
        if (line_length > 0) {
            outfile->write("\n", 1);
            line_length = 0;
        }
        outfile->write(">", 1);
        outfile->write(seq.c_str(), seq.length());
        outfile->write("\n", 1);
    }
    size_t s_pos = 0;
    while (true) {
        size_t length = std::min(50 - line_length, actual_length - s_pos);
        outfile->write(&(*string)[s_pos], length);
        s_pos += length;
        line_length += length;
        if (line_length == 50) {
            outfile->write("\n", 1);
            line_length = 0;
        }
        if (s_pos == actual_length) {
            return true;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void FastaSource::flushIn(uint64_t&) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

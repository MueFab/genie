/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"

#include <genie/util/runtime-exception.h>

#include <algorithm>
#include <istream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

// ---------------------------------------------------------------------------------------------------------------------

FastaReader::FastaReader(std::istream& fastaFile, std::istream& faiFile) : fai(faiFile), fasta(&fastaFile) {}

// ---------------------------------------------------------------------------------------------------------------------

std::set<std::string> FastaReader::getSequences() const { return fai.getSequences(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t FastaReader::getLength(const std::string& name) const { return fai.getLength(name); }

// ---------------------------------------------------------------------------------------------------------------------

std::string FastaReader::loadSection(const std::string& sequence, uint64_t start, uint64_t end) {
    auto startPos = fai.getFilePosition(sequence, start);
    std::string ret;
    ret.reserve(end - start);
    fasta->seekg(startPos);
    std::string buffer;
    while (getline(*fasta, buffer)) {
        start += buffer.size();
        if (start < end) {
            ret += buffer;
        } else if (start >= end) {
            ret += buffer.substr(0, buffer.size() - (start - end));
            break;
        }
    }
    std::transform(ret.begin(), ret.end(), ret.begin(), [](char x) -> char {return (char)toupper(x);});
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void FastaReader::index(std::istream& fasta, std::ostream& fai) {
    std::string buffer;
    FaiFile faiFile;
    FaiFile::FaiSequence seq;
    bool lastline = false;
    uint64_t nextoffset = 0;
    while (getline(fasta, buffer)) {
        UTILS_DIE_IF(buffer.empty(), "Empty line in fasta");
        if (buffer.front() == '>') {
            lastline = false;
            if (seq.offset != 0) {
                uint64_t tmp = seq.offset;
                seq.offset = nextoffset;
                faiFile.addSequence(seq);
                seq.offset = tmp;
            }
            seq.name = buffer.substr(1, buffer.find_first_of(' ') - 1);
            seq.offset += buffer.size() + 1;
            nextoffset = seq.offset;
            UTILS_DIE_IF(!getline(fasta, buffer), "Missing line in fasta");
            seq.length = 0;
            seq.linebases = buffer.size();
            seq.linewidth = buffer.size() + 1;
        } else {
            UTILS_DIE_IF(lastline, "Invalid fasta line length");
        }
        seq.length += buffer.size();
        seq.offset += buffer.size() + 1;
        if (buffer.size() != seq.linebases) {
            lastline = true;
        }
    }
    seq.offset = nextoffset;
    faiFile.addSequence(seq);
    fai << faiFile;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
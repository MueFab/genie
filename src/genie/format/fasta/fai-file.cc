/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/fasta/fai-file.h"
#include <istream>
#include <ostream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

// ---------------------------------------------------------------------------------------------------------------------

FaiFile::FaiSequence::FaiSequence(std::istream& stream) {
    const std::string ERROR_MSG = "Fai parsing failed";
    UTILS_DIE_IF(!std::getline(stream, name), ERROR_MSG);
    std::string buffer;
    UTILS_DIE_IF(!std::getline(stream, buffer), ERROR_MSG);
    length = std::stoll(buffer);
    UTILS_DIE_IF(!std::getline(stream, buffer), ERROR_MSG);
    offset = std::stoll(buffer);
    UTILS_DIE_IF(!std::getline(stream, buffer), ERROR_MSG);
    linebases = std::stol(buffer);
    UTILS_DIE_IF(!std::getline(stream, buffer), ERROR_MSG);
    linewidth = std::stol(buffer);
}

// ---------------------------------------------------------------------------------------------------------------------

FaiFile::FaiSequence::FaiSequence() : name(), length(0), offset(0), linebases(0), linewidth(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void FaiFile::addSequence(const FaiSequence& seq) {
    seqs.insert(std::make_pair(seq.name, seq));
    indices.insert(std::make_pair(indices.size(), seq.name));
}

// ---------------------------------------------------------------------------------------------------------------------

FaiFile::FaiFile(std::istream& stream) {
    while (stream.peek() != EOF) {
        FaiSequence seq(stream);
        addSequence(seq);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t FaiFile::getFilePosition(const std::string& sequence, uint64_t position) const {
    auto seq = seqs.find(sequence);
    UTILS_DIE_IF(seq == seqs.end(), "Unknown ref sequence");
    UTILS_DIE_IF(seq->second.length < position, "Reference position out of bounds");
    uint64_t offset = seq->second.offset;
    uint64_t fullbaselines = (position / seq->second.linebases);
    uint64_t fullbaselinesBytes = fullbaselines * seq->second.linewidth;
    uint64_t lastline = position % seq->second.linebases;
    uint64_t pos = offset + fullbaselinesBytes + lastline;
    return pos;
}

// ---------------------------------------------------------------------------------------------------------------------

std::map<size_t, std::string> FaiFile::getSequences() const {
    std::map<size_t, std::string> ret;
    for (const auto& s : indices) {
        ret.insert(std::make_pair(s.first, s.second));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool FaiFile::containsSequence(const std::string& seq) const { return seqs.find(seq) != seqs.end(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t FaiFile::getLength(const std::string& seq) const {
    UTILS_DIE_IF(!containsSequence(seq), "Unknown ref sequence");
    return seqs.at(seq).length;
}

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const FaiFile::FaiSequence& file) {
    stream << file.name << "\n";
    stream << file.length << "\n";
    stream << file.offset << "\n";
    stream << file.linebases << "\n";
    stream << file.linewidth << std::endl;
    return stream;
}

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const FaiFile& file) {
    std::map<uint64_t, FaiFile::FaiSequence> sortedMap;
    for (const auto& s : file.seqs) {
        sortedMap.insert(std::make_pair(s.second.offset, s.second));
    }
    for (const auto& s : sortedMap) {
        stream << s.second;
    }
    return stream;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

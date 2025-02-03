/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/fasta/reader.h"
#include <algorithm>
#include <istream>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

// ---------------------------------------------------------------------------------------------------------------------

FastaReader::FastaReader(std::istream& fastaFile, std::istream& faiFile, std::istream& sha256File, std::string _path)
    : hashFile(sha256File), fai(faiFile), fasta(&fastaFile), path(std::move(_path)) {}

// ---------------------------------------------------------------------------------------------------------------------

std::map<size_t, std::string> FastaReader::getSequences() const { return fai.getSequences(); }

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
    std::transform(ret.begin(), ret.end(), ret.begin(), [](char x) -> char { return static_cast<char>(toupper(x)); });
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
        if (buffer.empty()) {
            continue;
        }
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

void FastaReader::hash(const FaiFile& fai, std::istream& fasta, std::ostream& hash) {
    std::vector<std::pair<std::string, std::string>> hashes;
    for (const auto& s : fai.getSequences()) {
        auto pos = fai.getFilePosition(s.second, 0);
        auto length = fai.getLength(s.second);
        auto sha_value = Sha256File::hash(fasta, pos, length);
        hashes.emplace_back(s.second, sha_value);
    }
    Sha256File::write(hash, hashes);
}

// ---------------------------------------------------------------------------------------------------------------------

core::meta::Reference FastaReader::getMeta() const {
    std::string basename = path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - path.find_last_of('/') - 1);
    auto f = genie::util::make_unique<core::meta::external_ref::Fasta>(
        "file://" + path, core::meta::ExternalRef::ChecksumAlgorithm::SHA256);

    size_t id = 0;
    auto* f_ptr = f.get();
    core::meta::Reference ret(basename, 0, 0, 0, std::move(f), "");
    for (const auto& s : hashFile.getData()) {
        ret.addSequence(genie::core::meta::Sequence(s.first, fai.getLength(s.first), static_cast<uint16_t>(id++)));
        f_ptr->addChecksum(genie::util::fromHex(s.second));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

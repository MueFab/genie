/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/manager.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/format/fasta/reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

// ---------------------------------------------------------------------------------------------------------------------

Manager::Manager(std::istream& fasta, std::istream& fai, std::istream& sha, core::ReferenceManager* mgr,
                 std::string path)
    : ReferenceSource(mgr), reader(fasta, fai, sha, std::move(path)) {
    auto ref = generateRefHandles();
    int i = 0;
    for (auto& r : ref) {
        mgr->addRef(i++, std::move(r));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::map<size_t, std::string> Manager::getSequences() const { return reader.getSequences(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Manager::getLength(const std::string& seq) const { return reader.getLength(seq); }

// ---------------------------------------------------------------------------------------------------------------------

std::string Manager::getRef(const std::string& sequence, uint64_t start, uint64_t end) {
    std::lock_guard<std::mutex> guard(readerMutex);
    return reader.loadSection(sequence, start, end);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::unique_ptr<core::Reference>> Manager::generateRefHandles() {
    auto seqs = getSequences();
    std::vector<std::unique_ptr<core::Reference>> ret;
    for (const auto& s : seqs) {
        size_t length = getLength(s.second);
        ret.emplace_back(std::make_unique<Reference>(s.second, length, this));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

core::meta::Reference Manager::getMeta() const { return reader.getMeta(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

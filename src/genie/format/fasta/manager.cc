/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "manager.h"
#include <genie/util/make-unique.h>
#include <algorithm>
#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

// ---------------------------------------------------------------------------------------------------------------------

Manager::Manager(std::istream& fasta, std::istream& fai)
    : reader(fasta, fai) {
}

// ---------------------------------------------------------------------------------------------------------------------

std::set<std::string> Manager::getSequences() const { return reader.getSequences(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Manager::getLength(const std::string& seq) const { return reader.getLength(seq); }

// ---------------------------------------------------------------------------------------------------------------------

std::string Manager::getRef(const std::string& sequence, uint64_t start, uint64_t end) {
    std::lock_guard<std::mutex> guard(Manager::readerMutex);
    return reader.loadSection(sequence, start, end);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::unique_ptr<core::Reference>> Manager::generateRefHandles() {
    auto seqs = getSequences();
    std::vector<std::unique_ptr<core::Reference>> ret;
    for (const auto& s : seqs) {
        size_t length = getLength(s);
        ret.emplace_back(util::make_unique<Reference>(s, length, this));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
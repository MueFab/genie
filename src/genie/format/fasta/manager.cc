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
    : reader(fasta, fai), cacheList(CACHE_SIZE, std::make_pair("", 0)) {
    auto seqs = reader.getSequences();
    for (const auto& s : seqs) {
        auto length = reader.getLength(s);
        size_t num_chunks = (length - 1) / CHUNK_SIZE + 1;
        data[s].resize(num_chunks);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<std::string, uint64_t> Manager::updateCacheList(const std::string& seq, size_t id) {
    auto it = std::find(cacheList.begin(), cacheList.end(), std::make_pair(seq, id));
    if (it != cacheList.end()) {
        cacheList.erase(it);
        cacheList.insert(cacheList.begin(), std::make_pair(seq, id));
        return std::make_pair("", 0);
    } else {
        cacheList.insert(cacheList.begin(), std::make_pair(seq, id));
        auto to_remove = cacheList.back();
        cacheList.erase(cacheList.end() - 1);
        return to_remove;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<const std::string> Manager::getChunk(const std::string& sequence, size_t chunk) {
    auto to_remove = updateCacheList(sequence, chunk);
    if (!to_remove.first.empty()) {
        data[to_remove.first][to_remove.second].chunk = nullptr;
    }

    if (data[sequence][chunk].chunk) {
        return data[sequence][chunk].chunk;
    }

    uint64_t startPos = chunk * CHUNK_SIZE;
    uint64_t endPos = startPos + std::min(CHUNK_SIZE, getLength(sequence) - startPos);
    auto ret = std::make_shared<const std::string>(reader.loadSection(sequence, startPos, endPos));
    data[sequence][chunk].chunk = ret;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::set<std::string> Manager::getSequences() const { return reader.getSequences(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Manager::getLength(const std::string& seq) const { return reader.getLength(seq); }

// ---------------------------------------------------------------------------------------------------------------------

std::string Manager::getRef(const std::string& sequence, uint64_t start, uint64_t end) {
    size_t startChunk = start / CHUNK_SIZE;
    uint64_t offsetStart = start % CHUNK_SIZE;
    size_t endChunk = end / CHUNK_SIZE;
    uint64_t offsetEnd = CHUNK_SIZE - (end % CHUNK_SIZE);
    std::string ret;
    ret.reserve(end - start);
    for (size_t i = startChunk; i <= endChunk; ++i) {
        auto chunk = *getChunk(sequence, i);
        if (i == startChunk) {
            chunk = chunk.substr(offsetStart, chunk.length() - offsetStart);
        }
        if (i == endChunk) {
            chunk = chunk.substr(0, chunk.length() - offsetEnd);
        }
        ret += chunk;
    }
    return ret;
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
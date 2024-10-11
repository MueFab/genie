/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/format-importer.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

void FormatImporter::setClassifier(Classifier* _classifier) { classifier = _classifier; }

// ---------------------------------------------------------------------------------------------------------------------

bool FormatImporter::pump(uint64_t& id, std::mutex& lock) {
    record::Chunk chunk;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> guard(lock);
        chunk = classifier->getChunk();
        uint32_t segment_count = 0;
        for (const auto& r : chunk.getData()) {
            segment_count += static_cast<uint32_t>(r.getSegments().size());
        }
        if (chunk.getData().empty()) {
            segment_count = 1;
        }
        if (!chunk.getData().empty() || !chunk.getRefToWrite().empty()) {
            sec = {id, segment_count, true};
            id += segment_count;
        } else {
            bool dataLeft = pumpRetrieve(classifier);
            if (!dataLeft && !flushing) {
                classifier->flush();
                flushing = true;
                return true;
            }
            if (!dataLeft && flushing) {
                flushing = false;
                return false;
            }
        }
    }
    if (!chunk.getData().empty() || !chunk.getRefToWrite().empty()) {
        Source<record::Chunk>::flowOut(std::move(chunk), sec);
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void FormatImporter::flushIn(uint64_t& pos) { flushOut(pos); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "format-importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void FormatImporter::setClassifier(Classifier* _classifier) { classifier = _classifier; }

// ---------------------------------------------------------------------------------------------------------------------

bool FormatImporter::pump(size_t& id, std::mutex& lock) {
    record::Chunk chunk;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> guard(lock);
        bool flushing = classifier->isFlushing();
        chunk = classifier->getChunk();
        uint32_t segment_count = 0;
        for(const auto& r : chunk.getData()) {
            segment_count += r.getSegments().size();
        }
        if (!chunk.getData().empty()) {
            sec = {id, segment_count, true};
            id += segment_count;
        } else {
            bool dataLeft = pumpRetrieve(classifier);
            if (!dataLeft && !flushing) {
                classifier->flush();
                return true;
            }
            if (!dataLeft && flushing) {
                return false;
            }
        }
    }
    if (!chunk.getData().empty()) {
        Source<record::Chunk>::flowOut(std::move(chunk), sec);
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void FormatImporter::flushIn(size_t& pos) {flushOut(pos);}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
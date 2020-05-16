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

bool FormatImporter::pump(size_t& id) {
    record::Chunk chunk;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> guard(lock);
        bool flushing = classifier->isFlushing();
        chunk = classifier->getChunk();
        if (!chunk.empty()) {
            sec = {id, chunk.size(), false};
            id += chunk.size();
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
    if (!chunk.empty()) {
        Source<record::Chunk>::flowOut(std::move(chunk), sec);
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
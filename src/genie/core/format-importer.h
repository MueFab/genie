/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_H
#define GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit-payload.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include "classifier.h"
#include "record/record.h"

#include <mutex>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for importers of various file formats. Note that each importer has to convert to mpegg-records
 */
class FormatImporter : public util::OriginalSource, public util::Source<record::Chunk> {
   private:
    std::mutex lock;
    genie::core::Classifier* classifier;

   protected:
    virtual bool pumpRetrieve(genie::core::Classifier* _classifier) = 0;

   public:
    void setClassifier(genie::core::Classifier* _classifier) { classifier = _classifier; }
    bool pump(size_t& id) override {
        genie::core::record::Chunk chunk;
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
};

class FormatImporterCompressed : public util::Source<core::AccessUnitPayload>, public util::OriginalSource {};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
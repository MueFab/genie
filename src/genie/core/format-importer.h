/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_H
#define GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
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
    Classifier* classifier;  //!<
    bool flushing{false}; //!<
   protected:
    /**
     *
     * @param _classifier
     * @return
     */
    virtual bool pumpRetrieve(Classifier* _classifier) = 0;

   public:
    /**
     *
     * @param _classifier
     */
    void setClassifier(Classifier* _classifier);

    /**
     *
     * @param id
     * @return
     */
    bool pump(size_t& id, std::mutex& lock) override;

    /**
     *
     */
    void flushIn(size_t& pos) override;

    /**
     *
     */
    ~FormatImporter() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_IMPORTER_H_
#define SRC_GENIE_CORE_FORMAT_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <mutex>
#include "genie/core/access_unit.h"
#include "genie/core/classifier.h"
#include "genie/core/record/record.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for importers of various file formats. Note that each importer has to convert to mpegg-records
 */
class FormatImporter : public util::OriginalSource, public util::Source<record::Chunk> {
 private:
    Classifier* classifier;  //!< @brief
    bool flushing{false};    //!< @brief

 protected:
    /**
     * @brief
     * @param _classifier
     * @return
     */
    virtual bool pumpRetrieve(Classifier* _classifier) = 0;

 public:
    /**
     * @brief
     * @param _classifier
     */
    void setClassifier(Classifier* _classifier);

    /**
     * @brief
     * @param id
     * @param lock
     * @return
     */
    bool Pump(uint64_t& id, std::mutex& lock) override;

    /**
     * @brief
     */
    void FlushIn(uint64_t& pos) override;

    /**
     * @brief
     */
    ~FormatImporter() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

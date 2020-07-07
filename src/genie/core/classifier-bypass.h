/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CLASSIFIER_BYPASS_H
#define GENIE_CLASSIFIER_BYPASS_H

// ---------------------------------------------------------------------------------------------------------------------

#include "classifier.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ClassifierBypass : public Classifier {
    std::vector<record::Chunk> vec;  //!<
    bool flushing = false;           //!<

   public:
    /**
     *
     * @return
     */
    record::Chunk getChunk() override;

    /**
     *
     * @param c
     */
    void add(record::Chunk&& c) override;

    /**
     *
     */
    void flush() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CLASSIFIER_BYPASS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
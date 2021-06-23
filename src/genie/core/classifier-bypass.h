/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_
#define SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/classifier.h"

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

#endif  // SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

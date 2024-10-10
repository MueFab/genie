/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_
#define SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/classifier.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class ClassifierBypass : public Classifier {
    std::vector<record::Chunk> vec;  //!< @brief
    bool flushing = false;           //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    record::Chunk getChunk() override;

    /**
     * @brief
     * @param c
     */
    void add(record::Chunk&& c) override;

    /**
     * @brief
     */
    void flush() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

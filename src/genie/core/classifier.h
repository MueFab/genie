
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_H_
#define SRC_GENIE_CORE_CLASSIFIER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/chunk.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class Classifier {
 public:
    /**
     * @brief
     * @return
     */
    virtual record::Chunk getChunk() = 0;

    /**
     * @brief
     * @param c
     */
    virtual void add(record::Chunk&& c) = 0;

    /**
     * @brief
     */
    virtual void flush() = 0;

    /**
     * @brief
     */
    virtual ~Classifier() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

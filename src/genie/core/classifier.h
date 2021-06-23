
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_H_
#define SRC_GENIE_CORE_CLASSIFIER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/chunk.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class Classifier {
 public:
    /**
     *
     * @return
     */
    virtual record::Chunk getChunk() = 0;

    /**
     *
     * @param c
     */
    virtual void add(record::Chunk&& c) = 0;

    /**
     *
     */
    virtual void flush() = 0;

    /**
     *
     */
    virtual ~Classifier() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

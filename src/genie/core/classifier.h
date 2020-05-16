/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CLASSIFIER_H
#define GENIE_CLASSIFIER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/record.h"

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
     * @return
     */
    virtual bool isFlushing() const = 0;

    /**
     *
     */
    virtual ~Classifier() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CLASSIFIER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
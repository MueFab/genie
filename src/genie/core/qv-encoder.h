/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_ENCODER_H
#define GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/chunk.h>
#include <genie/util/drain.h>
#include <genie/util/source.h>
#include "access-unit.h"

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class QVEncoder {
   public:
    using QVCoded =
        std::tuple<std::unique_ptr<parameter::QualityValues>, AccessUnit::Descriptor, core::stats::PerfStats>;  //!<

    /**
     *
     * @param rec
     * @return
     */
    virtual QVCoded process(const record::Chunk& rec) = 0;

    /**
     * @brief For polymorphic destruction
     */
    virtual ~QVEncoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_QV_ENCODER_H_
#define SRC_GENIE_CORE_QV_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <tuple>
#include <utility>
#include "genie/core/access-unit.h"
#include "genie/core/record/chunk.h"
#include "genie/util/drain.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class QVEncoder {
 public:
    using QVCoded =
        std::tuple<std::unique_ptr<parameter::QualityValues>, AccessUnit::Descriptor, core::stats::PerfStats>;  //!<

    /**
     * @brief
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

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_QV_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

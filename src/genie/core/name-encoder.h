/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_ENCODER_H_
#define SRC_GENIE_CORE_NAME_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include <utility>
#include "genie/core/access-unit.h"
#include "genie/core/record/chunk.h"
#include "genie/util/drain.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class NameEncoder {
 public:
    /**
     *
     * @param rec
     * @return
     */
    virtual std::tuple<AccessUnit::Descriptor, core::stats::PerfStats> process(const record::Chunk& rec) = 0;

    /**
     * @brief For polymorphic destruction
     */
    virtual ~NameEncoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

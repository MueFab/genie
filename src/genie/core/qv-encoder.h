/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_ENCODER_H
#define GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/drain.h>
#include <genie/util/source.h>
#include "access-unit-raw.h"
#include "record/record.h"

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class QVEncoder {
   public:
    using QVCoded = std::pair<std::unique_ptr<parameter::QualityValues>, AccessUnitRaw::Descriptor>;  //!<

    /**
     *
     * @param rec
     * @return
     */
    virtual QVCoded process(const record::Chunk& rec) = 0;

    /**
     * @Brief For polymorphic destruction
     */
    ~QVEncoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
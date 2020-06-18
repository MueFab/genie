/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_NAME_DECODER_H
#define GENIE_NAME_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "access-unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class NameDecoder {
   public:
    /**
     *
     * @param desc
     * @return
     */
    virtual std::tuple<std::vector<std::string>, core::stats::PerfStats> process(AccessUnit::Descriptor& desc) = 0;

    /**
     * @Brief For polymorphic destruction
     */
    ~NameDecoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_DECODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
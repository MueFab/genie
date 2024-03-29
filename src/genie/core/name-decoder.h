/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_DECODER_H_
#define SRC_GENIE_CORE_NAME_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/access-unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class NameDecoder {
 public:
    /**
     * @brief
     * @param desc
     * @return
     */
    virtual std::tuple<std::vector<std::string>, core::stats::PerfStats> process(AccessUnit::Descriptor& desc) = 0;

    /**
     * @brief For polymorphic destruction
     */
    virtual ~NameDecoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

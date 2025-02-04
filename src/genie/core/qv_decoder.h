/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_QV_DECODER_H_
#define SRC_GENIE_CORE_QV_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>
#include "genie/core/access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

class QVDecoder {
 public:
    /**
     * @brief
     * @param param
     * @param ecigar
     * @param desc
     * @return
     */
    virtual std::tuple<std::vector<std::string>, core::stats::PerfStats> process(const parameter::QualityValues& param,
                                                                                 const std::vector<std::string>& ecigar,
                                                                                 const std::vector<uint64_t>& positions,
                                                                                 AccessUnit::Descriptor& desc) = 0;
    /**
     * @brief For polymorphic destruction
     */
    virtual ~QVDecoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_QV_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

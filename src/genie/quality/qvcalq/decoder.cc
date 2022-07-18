/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/qvcalq/decoder.h"
#include <string>
#include <tuple>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::process(
    const core::parameter::QualityValues& param, const std::vector<std::string>& ecigar_vec,
    const std::vector<uint64_t>& positions, core::AccessUnit::Descriptor& desc) {
    util::Watch watch;
    std::tuple<std::vector<std::string>, core::stats::PerfStats> qv;
    const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);
    (void)param_casted;
    (void)ecigar_vec;
    (void)positions;
    (void)desc;
    // param_casted.getCodebook(0).getEntries()

    std::get<1>(qv).addDouble("time-qvcalq", watch.check());
    return qv;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

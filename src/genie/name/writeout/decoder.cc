/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/name/writeout/decoder.h"
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::name::writeout {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::process(core::AccessUnit::Descriptor& desc) {
    std::tuple<std::vector<std::string>, core::stats::PerfStats> ret;
    util::Watch watch;
    std::string cur_name;
    if (desc.isEmpty()) {
        return ret;
    }
    while (!desc.get(0).end()) {
        auto cur_char = desc.get(0).pull();
        if (cur_char == '\0') {
            std::get<0>(ret).emplace_back(std::move(cur_name));
            cur_name.clear();
        } else {
            cur_name.push_back(static_cast<char>(cur_char));
        }
    }

    std::get<1>(ret).addDouble("time-namewriteout", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::name::writeout

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

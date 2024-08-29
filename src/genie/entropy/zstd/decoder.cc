/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/zstd/decoder.h"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include "genie/util/runtime-exception.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::zstd {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Decoder::process(
    const core::parameter::DescriptorSubseqCfg& param, core::AccessUnit::Descriptor& d, bool mmCoderEnabled) {
    (void)param;
    (void)mmCoderEnabled;
    util::Watch watch;
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> desc;
    std::get<0>(desc) = std::move(d);
    for (auto& subseq : std::get<0>(desc)) {
        if (subseq.isEmpty()) {
            continue;
        }
        auto d_id = subseq.getID();

        if (!subseq.isEmpty()) {
            std::get<1>(desc).addInteger("size-zstd-total-comp", subseq.getRawSize());
            std::get<1>(desc).addInteger("size-zstd-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                                             core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name +
                                             "-comp",
                                         subseq.getRawSize());
        }

        // TODO(Fabian) std::get<0>(desc).set(
        // d_id.second, decompress(gabac::EncodingConfiguration(std::move(conf0)), std::move(subseq), mmCoderEnabled));

        if (!std::get<0>(desc).get(d_id.second).isEmpty()) {
            std::get<1>(desc).addInteger("size-zstd-total-raw", std::get<0>(desc).get(d_id.second).getRawSize());
            std::get<1>(desc).addInteger("size-zstd-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                                             core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name +
                                             "-raw",
                                         std::get<0>(desc).get(d_id.second).getRawSize());
        }
    }
    std::get<1>(desc).addDouble("time-zstd", watch.check());
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

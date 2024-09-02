/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/zstd/decoder.h"

#include <zstd.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include "genie/util/runtime-exception.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::zstd {

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence decompress(core::AccessUnit::Subsequence&& data) {
    auto id = data.getID();

    uint8_t bytes = genie::core::range2bytes(core::getSubsequence(id).range);
    if (id.first == core::GenDesc::RNAME) {
        bytes = 1;
    }
    util::DataBlock in = data.move();
    size_t originalSize = ZSTD_getFrameContentSize(in.getData(), in.getRawSize());

    UTILS_DIE_IF(originalSize == ZSTD_CONTENTSIZE_ERROR, "ZSTD_getFrameContentSize failed");
    UTILS_DIE_IF(originalSize == ZSTD_CONTENTSIZE_UNKNOWN, "ZSTD_getFrameContentSize unknown");

    util::DataBlock out(originalSize, bytes);

    size_t decompressedSize = ZSTD_decompress(out.getData(), out.getRawSize(), in.getData(), in.getRawSize());

    UTILS_DIE_IF(ZSTD_isError(decompressedSize),
                 "ZSTD decompression failed: " + std::string(ZSTD_getErrorName(decompressedSize)));

    return {std::move(out), data.getID()};
}

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

        auto subseq_name = std::string();
        if (core::getDescriptor(std::get<0>(desc).getID()).tokentype) {
            subseq_name = core::getDescriptor(std::get<0>(desc).getID()).name;
        } else {
            subseq_name = core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                          core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name;
        }
        if (!subseq.isEmpty()) {
            std::get<1>(desc).addInteger("size-zstd-total-comp", subseq.getRawSize());
            std::get<1>(desc).addInteger("size-zstd-" + subseq_name +
                                             "-comp",
                                         subseq.getRawSize());
        }

        std::get<0>(desc).set(d_id.second, decompress(std::move(subseq)));

        if (!std::get<0>(desc).get(d_id.second).isEmpty()) {
            std::get<1>(desc).addInteger("size-zstd-total-raw", std::get<0>(desc).get(d_id.second).getRawSize());
            std::get<1>(desc).addInteger("size-zstd-" + subseq_name +
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

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/bsc/decoder.h"

#include <libbsc.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include "genie/util/runtime-exception.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence decompress(core::AccessUnit::Subsequence&& data) {
    auto id = data.getID();

    uint8_t bytes = genie::core::range2bytes(core::getSubsequence(id).range);
    if (id.first == core::GenDesc::RNAME) {
        bytes = 1;
    }
    util::DataBlock in = data.move();

    UTILS_DIE_IF(bsc_init(0) != LIBBSC_NO_ERROR, "bsc initialization failed");

    int originalSize = 0;
    int compressedSize = 0;
    UTILS_DIE_IF(bsc_block_info(static_cast<const unsigned char*>(in.getData()), LIBBSC_HEADER_SIZE, &compressedSize,
                                &originalSize, 0) != LIBBSC_NO_ERROR,
                 "bsc block info failed");

    util::DataBlock out(originalSize, bytes);

    UTILS_DIE_IF(bsc_decompress(static_cast<const unsigned char*>(in.getData()), in.getRawSize(),
                                static_cast<unsigned char*>(out.getData()), originalSize, 0) != LIBBSC_NO_ERROR,
                 "bsc decompression failed");

    return {std::move(out), data.getID()};
}

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

        auto subseq_name = std::string();
        if (core::getDescriptor(std::get<0>(desc).getID()).tokentype) {
            subseq_name = core::getDescriptor(std::get<0>(desc).getID()).name;
        } else {
            subseq_name = core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                          core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name;
        }
        if (!subseq.isEmpty()) {
            std::get<1>(desc).addInteger("size-bsc-total-comp", subseq.getRawSize());
            std::get<1>(desc).addInteger("size-bsc-" + subseq_name + "-comp", subseq.getRawSize());
        }

        std::get<0>(desc).set(d_id.second, decompress(std::move(subseq)));

        if (!std::get<0>(desc).get(d_id.second).isEmpty()) {
            std::get<1>(desc).addInteger("size-bsc-total-raw", std::get<0>(desc).get(d_id.second).getRawSize());
            std::get<1>(desc).addInteger("size-bsc-" + subseq_name + "-raw",
                                         std::get<0>(desc).get(d_id.second).getRawSize());
        }
    }
    std::get<1>(desc).addDouble("time-bsc", watch.check());
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

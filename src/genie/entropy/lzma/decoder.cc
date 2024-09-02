/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/lzma/decoder.h"

#include <lzma.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include "genie/util/runtime-exception.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::lzma {

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence decompress(core::AccessUnit::Subsequence&& data) {
    auto id = data.getID();

    uint8_t bytes = genie::core::range2bytes(core::getSubsequence(id).range);
    if (id.first == core::GenDesc::RNAME) {
        bytes = 1;
    }
    util::DataBlock in = data.move();

    lzma_stream strm = LZMA_STREAM_INIT;
    UTILS_DIE_IF(lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK, "lzma initialization failed");

    strm.next_in = static_cast<const unsigned char*>(in.getData());
    strm.avail_in = in.getRawSize();
    size_t outCapacity =
        in.getRawSize() * 2;  // Initial guess; typically this would be adjusted based on expected compression ratio
    util::DataBlock out(outCapacity, bytes);

    // Decompress in a loop, expanding the output buffer as needed
    while (true) {
        strm.next_out = static_cast<uint8_t*>(out.getData()) + strm.total_out;
        strm.avail_out = outCapacity - strm.total_out;

        auto ret = lzma_code(&strm, LZMA_FINISH);

        if (ret == LZMA_STREAM_END) {
            // Finished decompressing
            out.resize(strm.total_out);  // Resize to the actual size of decompressed data
            break;
        }

        UTILS_DIE_IF(ret != LZMA_OK, "LZMA decompression failed: " + std::to_string(ret));

        if (strm.avail_out == 0) {
            // Output buffer is full, so expand it
            outCapacity *= 2;
            out.resize(outCapacity);
        }
    }

    // Clean up
    lzma_end(&strm);

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
            std::get<1>(desc).addInteger("size-lzma-total-comp", subseq.getRawSize());
            std::get<1>(desc).addInteger("size-lzma-" + subseq_name + "-comp", subseq.getRawSize());
        }

        std::get<0>(desc).set(d_id.second, decompress(std::move(subseq)));

        if (!std::get<0>(desc).get(d_id.second).isEmpty()) {
            std::get<1>(desc).addInteger("size-lzma-total-raw", std::get<0>(desc).get(d_id.second).getRawSize());
            std::get<1>(desc).addInteger("size-lzma-" + subseq_name + "-raw",
                                         std::get<0>(desc).get(d_id.second).getRawSize());
        }
    }
    std::get<1>(desc).addDouble("time-lzma", watch.check());
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

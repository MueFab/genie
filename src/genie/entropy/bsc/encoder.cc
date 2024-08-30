/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/bsc/encoder.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

// ---------------------------------------------------------------------------------------------------------------------

core::EntropyEncoder::EntropyCoded Encoder::process(core::AccessUnit::Descriptor &desc) {
    EntropyCoded ret;
    util::Watch watch;
    std::get<1>(ret) = std::move(desc);
    for (auto &subdesc : std::get<1>(ret)) {
        if (!subdesc.isEmpty()) {
            // add compressed payload
            auto id = subdesc.getID();

            std::get<2>(ret).addInteger("size-zstd-total-raw", subdesc.getRawSize());
            std::get<2>(ret).addInteger("size-zstd-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" +
                                            core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name +
                                            "-raw",
                                        subdesc.getRawSize());

            // TODO(Fabian) std::get<1>(ret).set(id.second, compress(conf, std::move(subdesc)));

            if (!std::get<1>(ret).get(id.second).isEmpty()) {
                std::get<2>(ret).addInteger("size-zstd-total-comp", std::get<1>(ret).get(id.second).getRawSize());
                std::get<2>(ret).addInteger("size-zstd-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" +
                                                core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name +
                                                "-comp",
                                            std::get<1>(ret).get(id.second).getRawSize());
            }
        } else {
            // add empty payload
            std::get<1>(ret).set(subdesc.getID().second,
                                 core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
        }
    }
    // TODO(Fabian) configSet.storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));
    std::get<2>(ret).addDouble("time-zstd", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(bool _writeOutStreams) : writeOutStreams(_writeOutStreams) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

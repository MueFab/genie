/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_ENCODER_H
#define GENIE_GABAC_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
#include <genie/core/entropy-encoder.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/util/make-unique.h>
#include <genie/util/watch.h>
#include "gabac-seq-conf-set.h"

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Module to compress raw access units into blockpayloads using GABAC
 */
class Encoder : public core::EntropyEncoder {
   private:
    /**
     * @brief Run the actual gabac compression
     * @param conf GABAC configuration to use
     * @param in Uncompressed data
     * @param out Where to put compressed data. A set of transformed subsequences is generated
     */
    static core::AccessUnit::Subsequence compress(const gabac::EncodingConfiguration& conf,
                                                  core::AccessUnit::Subsequence&& in);

    static core::AccessUnit::Descriptor compressTokens(const gabac::EncodingConfiguration& conf0,
                                                       core::AccessUnit::Descriptor&& in);

   public:
    GabacSeqConfSet configSet;  //!< @brief Config set to use. In contrast to decompression it's static over time

    EntropyCoded process(core::AccessUnit::Descriptor& desc) override {
        EntropyCoded ret;
        util::Watch watch;
        std::get<1>(ret) = std::move(desc);
        if (!getDescriptor(std::get<1>(ret).getID()).tokentype) {
            for (auto& subdesc : std::get<1>(ret)) {
                if (!subdesc.isEmpty()) {
                    const auto& conf = configSet.getConfAsGabac(subdesc.getID());
                    // add compressed payload
                    auto id = subdesc.getID();

                    if (!subdesc.isEmpty()) {
                        std::get<2>(ret).addInteger("size-gabac-total-raw", subdesc.getRawSize());
                        std::get<2>(ret).addInteger(
                            "size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" + core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name + "-raw",
                            subdesc.getRawSize());
                    }

                    std::get<1>(ret).set(id.second, compress(conf, std::move(subdesc)));

                    if (!std::get<1>(ret).get(id.second).isEmpty()) {
                        std::get<2>(ret).addInteger("size-gabac-total-comp",
                                                    std::get<1>(ret).get(id.second).getRawSize());
                        std::get<2>(ret).addInteger(
                            "size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-" + core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name + "-comp",
                            std::get<1>(ret).get(id.second).getRawSize());
                    }
                } else {
                    // add empty payload
                    std::get<1>(ret).set(subdesc.getID().second,
                                         core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
                }
            }
            configSet.storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));
        } else {
            size_t size = 0;
            for (const auto& s : std::get<1>(ret)) {
                size += s.getNumSymbols() * sizeof(uint32_t);
            }
            std::string name = getDescriptor(std::get<1>(ret).getID()).name;
            const auto& conf = configSet.getConfAsGabac({std::get<1>(ret).getID(), 0});
            std::get<1>(ret) = compressTokens(conf, std::move(std::get<1>(ret)));
            configSet.storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));

            if (size) {
                std::get<2>(ret).addInteger("size-gabac-total-raw", size);
                std::get<2>(ret).addInteger("size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-raw",
                                            size);
                std::get<2>(ret).addInteger("size-gabac-total-comp", std::get<1>(ret).begin()->getRawSize());
                std::get<2>(ret).addInteger(
                    "size-gabac-" + core::getDescriptor(std::get<1>(ret).getID()).name + "-comp",
                    std::get<1>(ret).begin()->getRawSize());
            }
        }
        std::get<2>(ret).addDouble("time-gabac", watch.check());
        return ret;
    }
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

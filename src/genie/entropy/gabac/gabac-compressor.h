/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_COMPRESSOR_H
#define GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
#include <genie/core/entropy-encoder.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/util/make-unique.h>
#include "gabac-seq-conf-set.h"

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Module to compress raw access units into blockpayloads using GABAC
 */
class GabacCompressor : public core::EntropyEncoder {
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
        ret.second = std::move(desc);
        if (!getDescriptor(ret.second.getID()).tokentype) {
            for (auto& subdesc : ret.second) {
                if (!subdesc.isEmpty()) {
                    const auto& conf = configSet.getConfAsGabac(subdesc.getID());
                    // add compressed payload
                    auto id = subdesc.getID();
                    ret.second.set(id.second, compress(conf, std::move(subdesc)));
                } else {
                    // add empty payload
                    ret.second.set(subdesc.getID().second,
                                   core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
                }
            }
            configSet.storeParameters(ret.second.getID(),ret.first);
        } else {
            size_t size = 0;
            for (const auto& s : ret.second) {
                size += s.getNumSymbols() * sizeof(uint32_t);
            }
            std::string name = getDescriptor(ret.second.getID()).name;
            const auto& conf = configSet.getConfAsGabac({ret.second.getID(), 0});
            ret.second = compressTokens(conf, std::move(ret.second));
            configSet.storeParameters(ret.second.getID(),ret.first);
        }
        return ret;
    }
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

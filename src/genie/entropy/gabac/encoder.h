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

// ---------------------------------------------------------------------------------------------------------------------

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
     * @param dependency Dependency sequence (optional)
     */
    static core::AccessUnit::Subsequence compress(const gabac::EncodingConfiguration& conf,
                                                  core::AccessUnit::Subsequence&& in,
                                                  util::DataBlock* const dependency = nullptr);

    /**
     *
     * @param conf0
     * @param in
     * @return
     */
    static core::AccessUnit::Descriptor compressTokens(const gabac::EncodingConfiguration& conf0,
                                                       core::AccessUnit::Descriptor&& in);

   public:
    GabacSeqConfSet configSet;  //!< @brief Config set to use. In contrast to decompression it's static over time

    /**
     *
     * @param desc
     * @return
     */
    EntropyCoded process(core::AccessUnit::Descriptor& desc) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

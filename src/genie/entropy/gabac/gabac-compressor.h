/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_COMPRESSOR_H
#define GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit-payload.h>
#include <genie/core/entropy-encoder.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/util/make-unique.h>
#include "gabac-seq-conf-set.h"
#include "genie-gabac-output-stream.h"

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
    static core::AccessUnitPayload::SubsequencePayload compress(const gabac::EncodingConfiguration& conf,
                                                                core::AccessUnitRaw::Subsequence&& in);

   public:
    GabacSeqConfSet configSet;  //!< @brief Config set to use. In contrast to decompression it's static over time

    /**
     * @brief Process one raw access unit
     * @param raw_aus Input data
     * @param id Block identifier (for multithreading).
     */
    void flowIn(core::AccessUnitRaw&& raw_aus, size_t id) override;

    /**
     * @brief Propagate end of data signal
     */
    void dryIn() override;
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

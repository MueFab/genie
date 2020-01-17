/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_DECOMPRESSOR_H
#define GENIE_GABAC_DECOMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit-payload.h>
#include <genie/core/entropy-decoder.h>
#include <genie/util/make-unique.h>
#include "gabac-seq-conf-set.h"

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Module to decompress a BlockPayload back into a raw access unit using Gabac
 */
class GabacDecompressor : public core::EntropyDecoder {
   private:
    /**
     * @brief Execute gabac library
     * @param conf Gabac configuration to use
     * @param in Compressed set of transformed sequences
     * @param out Where to put uncompressed sequence
     */
    static core::AccessUnitRaw::Subsequence decompress(const gabac::EncodingConfiguration& conf,
                                                       core::AccessUnitPayload::SubsequencePayload&& in);

   public:
    /**
     * @brief Process BlockPayload and execute gabac decompression
     * @param payloadSet Input payload
     * @param id Block identification (for multithreading)
     */
    void flowIn(core::AccessUnitPayload&& payloadSet, size_t id) override;

    /**
     * @brief Propagate end of data signal
     */
    void dryIn() override;
};

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_DECOMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
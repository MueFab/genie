/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_DECOMPRESSOR_H
#define GENIE_GABAC_DECOMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/make_unique.h>

#include <backbone/mpegg-decompressor.h>
#include "gabac-seq-conf-set.h"

namespace genie {
namespace gabac {

/**
 * @brief Module to decompress a BlockPayload back into a raw access unit using Gabac
 */
class GabacDecompressor : public genie::MpeggDecompressor {
   private:
    /**
     * @brief Execute gabac library
     * @param conf Gabac configuration to use
     * @param in Compressed set of transformed sequences
     * @param out Where to put uncompressed sequence
     */
    static genie::MpeggRawAu::SubDescriptor decompress(const gabac::EncodingConfiguration& conf,
                                                       genie::BlockPayloadSet::SubsequencePayload&& in);

   public:
    /**
     * @brief Process BlockPayload and execute gabac decompression
     * @param payloadSet Input payload
     * @param id Block identification (for multithreading)
     */
    void flowIn(genie::BlockPayloadSet&& payloadSet, size_t id) override;

    /**
     * @brief Propagate end of data signal
     */
    void dryIn() override;
};

}  // namespace gabac
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_DECOMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
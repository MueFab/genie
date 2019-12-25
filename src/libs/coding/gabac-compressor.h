/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_COMPRESSOR_H
#define GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <gabac/gabac.h>
#include <util/make_unique.h>
#include "gabac-seq-conf-set.h"
#include "genie-gabac-output-stream.h"
#include "mpegg-compressor.h"

/**
 * @brief Module to compress raw access units into blockpayloads using GABAC
 */
class GabacCompressor : public MpeggCompressor {
   private:
    /**
     * @brief Run the actual gabac compression
     * @param conf GABAC configuration to use
     * @param in Uncompressed data
     * @param out Where to put compressed data. A set of transformed subsequences is generated
     */
    static void compress(const gabac::EncodingConfiguration &conf, MpeggRawAu::SubDescriptor *in,
                         BlockPayloadSet::SubsequencePayload *out);

   public:
    GabacSeqConfSet configSet;  //!< @brief Config set to use. In contrast to decompression it's static over time

    /**
     * @brief Process one raw access unit
     * @param raw_aus Input data
     * @param id Block identifier (for multithreading).
     */
    void flowIn(std::unique_ptr<MpeggRawAu> raw_aus, size_t id) override;

    /**
     * @brief Propagate end of data signal
     */
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

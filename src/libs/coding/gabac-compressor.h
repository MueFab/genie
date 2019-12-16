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
 *
 */
class GabacCompressor : public MpeggCompressor {
   private:
    /**
     *
     * @param conf
     * @param in
     * @param out
     */
    static void compress(const gabac::EncodingConfiguration& conf, gabac::DataBlock* in,
                         std::vector<gabac::DataBlock>* out);

   public:
    GabacSeqConfSet configSet;  //!<

    /**
     *
     * @param raw_aus
     * @param id
     */
    void flowIn(std::unique_ptr<MpeggRawAu> raw_aus, size_t id) override;

    /**
     *
     */
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_COMPRESSOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

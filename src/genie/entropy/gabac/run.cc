/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/run.h"
#include <cstdint>
#include "genie/entropy/gabac/decode-desc-subseq.h"
#include "genie/entropy/gabac/encode-desc-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

uint64_t run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode) {
    if (decode) {
        return gabac::decodeDescSubsequence(conf, enConf);
    } else {
        return gabac::encodeDescSubsequence(conf, enConf);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

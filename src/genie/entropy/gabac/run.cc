/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "run.h"

#include "decode-desc-subseq.h"
#include "encode-desc-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

unsigned long run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode) {
    if (decode) {
        return gabac::decodeDescSubsequence(conf, enConf);
    } else {
        return gabac::encodeDescSubsequence(conf, enConf);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
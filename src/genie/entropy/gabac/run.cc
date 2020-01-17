/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "run.h"

#include "decoding.h"
#include "encoding.h"
namespace genie {
namespace entropy {
namespace gabac {

void run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode) {
    if (decode) {
        gabac::decode(conf, enConf);
    } else {
        gabac::encode(conf, enConf);
    }
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
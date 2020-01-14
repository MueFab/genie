/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "run.h"

#include "decoding.h"
#include "encoding.h"
namespace genie {
namespace gabac {

void run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode) {
    if (decode) {
        gabac::decode(conf, enConf);
    } else {
        gabac::encode(conf, enConf);
    }
}
}  // namespace gabac
}  // namespace genie

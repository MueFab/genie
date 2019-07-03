/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "run.h"

#include "encoding.h"
#include "decoding.h"

namespace gabac {
void run(
        const IOConfiguration& conf,
        const EncodingConfiguration& enConf,
        bool decode
) {
    if(decode) {
        gabac::decode(conf, enConf);
    } else {
        gabac::encode(conf, enConf);
    }
}

}  // namespace gabac

/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_DECODING_H_
#define GABAC_DECODING_H_

namespace genie {
namespace gabac {

struct EncodingConfiguration;
struct IOConfiguration;

void decode(const IOConfiguration& ioConf, const EncodingConfiguration& enConf);

}  // namespace gabac
}  // namespace genie

#endif  // GABAC_DECODING_H_

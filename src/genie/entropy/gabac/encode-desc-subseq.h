/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_ENCODE_DESC_SUBSEQ_H_
#define GABAC_ENCODE_DESC_SUBSEQ_H_

namespace genie {
namespace entropy {
namespace gabac {

struct IOConfiguration;
struct EncodingConfiguration;

unsigned long encodeDescSubsequence(const IOConfiguration& conf, const EncodingConfiguration& enConf);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_ENCODE_DESC_SUBSEQ_H_

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_ENCODE_CABAC_H_
#define GABAC_ENCODE_CABAC_H_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <genie/entropy/paramcabac/transformed-seq.h>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

void encode_cabac(const paramcabac::TransformedSeq &conf,
                  util::DataBlock* symbols,
                  size_t maxsize = std::numeric_limits<size_t>::max());

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_ENCODE_CABAC_H_

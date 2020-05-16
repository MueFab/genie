/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "constants.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

#include "decode-cabac.h"
#include "encode-cabac.h"
#include "equality-subseq-transform.h"
#include "match-subseq-transform.h"
#include "rle-subseq-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

// ------------------------------------------------------------------------------

const TransformationProperties &getTransformation(const gabac::DescSubseqTransformationId &id) {
    static const std::vector<TransformationProperties> trnsfInfo = {
        {"no_transform",  // Name
         {},
         {"out"},  // StreamNames
         {0},      // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSubseqs) {
             transformedSubseqs->resize(1);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSubseqs) {
             transformedSubseqs->resize(1);
         }},
        {"equality_coding",  // Name
         {},
         {"raw_symbols", "eq_flags"},  // StreamNames
         {0, 1},                       // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSubseqs) {
             transformedSubseqs->resize(2);
             (*transformedSubseqs)[1] = util::DataBlock(0, 1);
             gabac::transformEqualityCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSubseqs) {
             gabac::inverseTransformEqualityCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
             transformedSubseqs->resize(1);
         }},
        {"match_coding",  // Name
         {"window_size"},
         {"raw_values", "pointers", "lengths"},  // StreamNames
         {0, 4, 4},                              // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSubseqs) {
             transformedSubseqs->resize(3);
             assert(param[0] <= std::numeric_limits<uint32_t>::max());
             (*transformedSubseqs)[1] = util::DataBlock(0, 4);
             (*transformedSubseqs)[2] = util::DataBlock(0, 4);
             gabac::transformMatchCoding(static_cast<uint32_t>(param[0]), &(*transformedSubseqs)[0],
                                         &(*transformedSubseqs)[1], &(*transformedSubseqs)[2]);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSubseqs) {
             gabac::inverseTransformMatchCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1],
                                                &(*transformedSubseqs)[2]);
             transformedSubseqs->resize(1);
         }},
        {"rle_coding",  // Name
         {"guard"},
         {"raw_values", "lengths"},  // StreamNames
         {0, 1},                     // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSubseqs) {
             transformedSubseqs->resize(2);
             (*transformedSubseqs)[1] = util::DataBlock(0, 1);
             gabac::transformRleCoding(param[0], &(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
         },
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSubseqs) {
             gabac::inverseTransformRleCoding(param[0], &(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
             transformedSubseqs->resize(1);
         }}};
    return trnsfInfo[unsigned(id)];
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

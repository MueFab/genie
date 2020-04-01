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
#include "equality-coding.h"
#include "match-coding.h"
#include "rle-coding.h"

namespace genie {
namespace entropy {
namespace gabac {

// ------------------------------------------------------------------------------

const TransformationProperties &getTransformation(const gabac::SequenceTransformationId &id) {
    static const std::vector<TransformationProperties> transformationInformation = {
        {"no_transform",  // Name
         {},
         {"out"},  // StreamNames
         {0},      // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(1);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(1);
         }},
        {"equality_coding",  // Name
         {},
         {"raw_symbols", "eq_flags"},  // StreamNames
         {0, 1},                       // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(2);
             (*transformedSequences)[1] = util::DataBlock(0, 1);
             gabac::transformEqualityCoding(&(*transformedSequences)[0], &(*transformedSequences)[1]);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSequences) {
             gabac::inverseTransformEqualityCoding(&(*transformedSequences)[0], &(*transformedSequences)[1]);
             transformedSequences->resize(1);
         }},
        {"match_coding",  // Name
         {"window_size"},
         {"raw_values", "pointers", "lengths"},  // StreamNames
         {0, 4, 4},                              // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(3);
             assert(param[0] <= std::numeric_limits<uint32_t>::max());
             (*transformedSequences)[1] = util::DataBlock(0, 4);
             (*transformedSequences)[2] = util::DataBlock(0, 4);
             gabac::transformMatchCoding(static_cast<uint32_t>(param[0]), &(*transformedSequences)[0],
                                         &(*transformedSequences)[1], &(*transformedSequences)[2]);
         },
         [](const std::vector<uint64_t> &, std::vector<util::DataBlock> *const transformedSequences) {
             gabac::inverseTransformMatchCoding(&(*transformedSequences)[0], &(*transformedSequences)[1],
                                                &(*transformedSequences)[2]);
             transformedSequences->resize(1);
         }},
        {"rle_coding",  // Name
         {"guard"},
         {"raw_values", "lengths"},  // StreamNames
         {0, 1},                     // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(2);
             (*transformedSequences)[1] = util::DataBlock(0, 1);
             gabac::transformRleCoding(param[0], &(*transformedSequences)[0], &(*transformedSequences)[1]);
         },
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSequences) {
             gabac::inverseTransformRleCoding(param[0], &(*transformedSequences)[0], &(*transformedSequences)[1]);
             transformedSequences->resize(1);
         }},
         /*/* RESTRUCT-DISABLE
        {"paramcabac",  // Name
         {"binarization_id", "binarization_parameter", "context_selection_id", "word_size"},
         {"sequence"},  // StreamNames
         {0},           // WordSizes (0: non fixed current stream wordsize)
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(1);
             gabac::encode_cabac(gabac::BinarizationId(param[0]), {static_cast<unsigned>(param[1])},
                                 gabac::ContextSelectionId(param[2]), &(*transformedSequences)[0]);
         },
         [](const std::vector<uint64_t> &param, std::vector<util::DataBlock> *const transformedSequences) {
             transformedSequences->resize(1);
             gabac::decode_cabac(gabac::BinarizationId(param[0]), {static_cast<unsigned>(param[1])},
                                 gabac::ContextSelectionId(param[2]), static_cast<uint8_t>(param[3]),
                                 &(*transformedSequences)[0]);
         }}*/
         };
    return transformationInformation[unsigned(id)];
}

bool BinarizationProperties::sbCheck(uint64_t minv, uint64_t maxv, uint64_t parameter) const {
    if (isSigned) {
        return int64_t(minv) >= int64_t(this->min(parameter)) && int64_t(maxv) <= int64_t(this->max(parameter));
    }
    return minv >= this->min(parameter) && maxv <= this->max(parameter);
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
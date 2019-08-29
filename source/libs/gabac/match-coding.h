/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_MATCH_CODING_H_
#define GABAC_MATCH_CODING_H_

#include <cstdint>

namespace gabac {

class DataBlock;

void transformMatchCoding(uint32_t windowSize, DataBlock *rawValues,
                          DataBlock *pointers, DataBlock *lengths);

void inverseTransformMatchCoding(DataBlock *rawValues, DataBlock *pointers,
                                 DataBlock *lengths);

}  // namespace gabac

#endif  // GABAC_MATCH_CODING_H_

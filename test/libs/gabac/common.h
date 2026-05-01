#ifndef GABAC_TESTS_COMMON_H_
#define GABAC_TESTS_COMMON_H_

#include "genie/util/data_block.h"

namespace gabac_tests {

void FillVectorRandomUniform(uint64_t min, uint64_t max,
                             genie::util::DataBlock *vector);
void FillVectorRandomGeometric(genie::util::DataBlock *vector);

}  // namespace gabac_tests

#endif  // GABAC_TESTS_COMMON_H_

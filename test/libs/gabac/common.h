#ifndef GABAC_TESTS_COMMON_H_
#define GABAC_TESTS_COMMON_H_

#include <util/data-block.h>

namespace gabac_tests {

void fillVectorRandomUniform(uint64_t min, uint64_t max, util::DataBlock *vector);
void fillVectorRandomGeometric(util::DataBlock *vector);

}  // namespace gabac_tests

#endif  // GABAC_TESTS_COMMON_H_

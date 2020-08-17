#ifndef GABAC_TESTS_COMMON_H_
#define GABAC_TESTS_COMMON_H_

#include <genie/util/data-block.h>

namespace gabac_tests {

void fillVectorRandomUniform(uint64_t min, uint64_t max, genie::util::DataBlock *vector);
void fillVectorRandomGeometric(genie::util::DataBlock *vector);

}  // namespace gabac_tests

#endif  // GABAC_TESTS_COMMON_H_

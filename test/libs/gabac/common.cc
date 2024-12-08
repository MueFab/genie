#include "common.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace gabac_tests {

void FillVectorRandomUniform(uint64_t min, const uint64_t max,
                             genie::util::DataBlock *const vector) {
  // First create an instance of an engine.
  std::random_device rnd_device;
  // Specify the engine and distribution.
  const auto seed = rnd_device();
  std::mt19937_64 mersenne_engine{seed};  // Generates random integers;
  if (min <= max) {
    std::cout << "fillVectorRandomInterval: min: " << min << "; max: " << max
              << "; seed: " << seed << std::endl;
  } else {
    std::cout << "fillVectorRandomInterval: min: " << static_cast<int64_t>(min)
              << "; max: " << static_cast<int64_t>(max) << "; seed: " << seed
              << std::endl;
  }
  std::uniform_int_distribution<uint64_t> dist(0, max - min);

  std::generate(
      vector->begin(), vector->end(), [&dist, &mersenne_engine, &min] {
        return dist(mersenne_engine) +
               min;  // to keep the diff values within the 64-bit signed range
      });
}

void FillVectorRandomGeometric(genie::util::DataBlock *const vector) {
  // First create an instance of an engine.
  std::random_device rnd_device;
  // Specify the engine and distribution.
  const auto seed = rnd_device();
  std::mt19937_64 mersenne_engine{seed};  // Generates random integers
  std::cout << "fillVectorRandomGeometric: type: " << vector->GetWordSize()
            << "; seed: " << seed << std::endl;

  std::geometric_distribution<uint64_t> dist(0.05);

  std::generate(vector->begin(), vector->end(), [&dist, &mersenne_engine] {
    return dist(mersenne_engine);  // to keep the diff values within the 64-bit
                                   // signed range
  });
}

}  // namespace gabac_tests

/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

// LUT encoding is also using a different algorithms for wordsizes 1 and 2. For these word sizes, the input values are
// in a small range and the algorithm can use a fast array look up when building the table. For greater word sizes this
// cannot be assumed so a slower hash table has to used

#include "lut-transform.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "block-stepper.h"
#include "data-block.h"

namespace gabac {

const size_t MAX_LUT_SIZE = 1u << 20u;  // 8MB table

// Inplace not reasonable, because symbols stream is reused 100% of time and lut
// is much smaller
static void inferLut0(const DataBlock& symbols, std::vector<std::pair<uint64_t, uint64_t>>* const lut,
                      DataBlock* const fastlut, DataBlock* const inverseLut) {
    uint64_t maxValue = 0;

    // At some point it is more efficient to use an hashmap instead of an array
    const uint64_t CTR_THRESHOLD = 1000000;

    // Wordsize 1 and 2 are small enough to use the array every time
    if (symbols.getWordSize() == 1) {
        maxValue = std::numeric_limits<uint8_t>::max();
    } else if (symbols.getWordSize() == 2) {
        maxValue = std::numeric_limits<uint16_t>::max();
    } else {
        // For greater wordsizes: find max and check if small enough for array
        // counter
        for (size_t i = 0; i < symbols.size(); ++i) {
            uint64_t val = symbols.get(i);
            if (val > maxValue) {
                maxValue = val;
            }
            if (val >= CTR_THRESHOLD) {
                break;
            }
        }
        // Step 1: basic mapping for order 0. All symbols are now in a dense
        // interval starting [0...N]
    }

    // Clear
    lut->clear();
    inverseLut->clear();
    if (symbols.empty()) {
        return;
    }

    std::vector<std::pair<uint64_t, uint64_t>> freqVec;

    if (maxValue < CTR_THRESHOLD) {
        std::vector<uint64_t> freq(maxValue + 1);
        BlockStepper r = symbols.getReader();
        while (r.isValid()) {
            uint64_t symbol = r.get();
            freq[symbol]++;
            r.inc();
        }
        for (size_t i = 0; i < freq.size(); ++i) {
            if (freq[i]) {
                freqVec.emplace_back(uint64_t(i), freq[i]);
            }
        }
    } else {
        std::unordered_map<uint64_t, uint64_t> freq;
        BlockStepper r = symbols.getReader();
        while (r.isValid()) {
            uint64_t symbol = r.get();
            freq[symbol]++;
            if (freq.size() >= MAX_LUT_SIZE) {
                GABAC_DIE("LUT too big");
            }
            r.inc();
        }
        std::copy(freq.begin(), freq.end(), std::back_inserter(freqVec));
    }

    // Sort symbol frequencies in descending order
    std::sort(freqVec.begin(), freqVec.end(),
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) {
                  if (a.second > b.second) {
                      return true;
                  }
                  if (a.second < b.second) {
                      return false;
                  }
                  return a.first < b.first;
              });

    for (const auto& symbol : freqVec) {
        lut->emplace_back(symbol.first, inverseLut->size());
        inverseLut->emplace_back(symbol.first);
    }

    // Sort symbols
    std::sort(lut->begin(), lut->end(),
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) {
                  if (a.first < b.first) {
                      return true;
                  }
                  if (a.first > b.first) {
                      return false;
                  }
                  return a.second > b.second;
              });

    if (maxValue < CTR_THRESHOLD) {
        fastlut->resize(maxValue + 1);
        for (auto p : *lut) {
            (*fastlut).set(p.first, p.second);
        }
    }
}

static uint64_t lut0SingleTransform(const std::vector<std::pair<uint64_t, uint64_t>>& lut0, uint64_t symbol) {
    auto it = std::lower_bound(lut0.begin(), lut0.end(), std::make_pair(symbol, uint(0)),
                               [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) {
                                   return a.first < b.first;
                               });
    assert(it != lut0.end());
    assert(it->first == symbol);
    return it->second;
}

static uint64_t lut0SingleTransformFast(const DataBlock& lut0, uint64_t symbol) { return lut0.get(symbol); }

static void transformLutTransform_core(const size_t ORDER, const std::vector<std::pair<uint64_t, uint64_t>>& lut0,
                                       const DataBlock& fastlut, const DataBlock& lut,
                                       DataBlock* const transformedSymbols) {
    assert(transformedSymbols != nullptr);

    if (transformedSymbols->empty()) {
        return;
    }

    std::vector<uint64_t> lastSymbols(ORDER + 1, 0);

    BlockStepper r = transformedSymbols->getReader();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        // Update history
        for (size_t j = ORDER; j > 0; --j) {
            lastSymbols[j] = lastSymbols[j - 1];
        }
        if (!fastlut.empty()) {
            lastSymbols[0] = lut0SingleTransformFast(fastlut, symbol);
        } else {
            lastSymbols[0] = lut0SingleTransform(lut0, symbol);
        }

        // Transform
        uint64_t transformed = lastSymbols[0];
        if (ORDER > 0) {
            // Compute position
            size_t index = 0;
            for (size_t j = ORDER; j > 0; --j) {
                index *= lut0.size();
                index += lastSymbols[j];
            }
            index *= lut0.size();
            index += lastSymbols[0];
            transformed = lut.get(index);
        }
        r.set(transformed);
        r.inc();
    }
}

static void inverseTransformLutTransform_core(const size_t ORDER, DataBlock* const symbols,
                                              DataBlock* const inverseLut0, DataBlock* const inverseLut) {
    assert(symbols != nullptr);

    std::vector<uint64_t> lastSymbols(ORDER + 1, 0);

    BlockStepper r = symbols->getReader();

    // Do the LUT transform
    while (r.isValid()) {
        uint64_t symbol = r.get();
        // Update history
        for (size_t j = ORDER; j > 0; --j) {
            lastSymbols[j] = lastSymbols[j - 1];
        }
        lastSymbols[0] = static_cast<uint64_t>(symbol);

        if (ORDER == 0) {
            r.set(inverseLut0->get(lastSymbols[0]));
            r.inc();
            continue;
        }

        // Compute position
        size_t index = 0;
        for (size_t j = ORDER; j > 0; --j) {
            index *= inverseLut0->size();
            index += lastSymbols[j];
        }
        index *= inverseLut0->size();
        index += lastSymbols[0];

        // Transform
        uint64_t unTransformed = inverseLut->get(index);
        lastSymbols[0] = unTransformed;
        r.set(inverseLut0->get(unTransformed));
        r.inc();
    }
}

void inferLut(const size_t ORDER, const DataBlock& symbols, std::vector<std::pair<uint64_t, uint64_t>>* const lut0,
              DataBlock* const fastlut, DataBlock* const inverseLut0, DataBlock* const lut1,
              DataBlock* const inverseLut1) {
    // Clear
    lut1->clear();
    inverseLut1->clear();

    inferLut0(symbols, lut0, fastlut, inverseLut0);

    if (symbols.empty()) {
        return;
    }

    if (ORDER == 0) {
        return;
    }

    size_t size = 1;
    for (size_t i = 0; i < ORDER + 1; ++i) {
        size *= inverseLut0->size();
    }

    if (size >= MAX_LUT_SIZE) {
        lut0->clear();
        return;
    }

    std::vector<std::pair<uint64_t, uint64_t>> ctr(size, {std::numeric_limits<uint64_t>::max(), 0});
    std::vector<uint64_t> lastSymbols(ORDER + 1, 0);

    BlockStepper r = symbols.getReader();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        // Update history
        for (size_t j = ORDER; j > 0; --j) {
            lastSymbols[j] = lastSymbols[j - 1];
        }

        // Translate symbol into order1 symbol
        uint64_t narrowedSymbol = symbol;
        lastSymbols[0] = lut0SingleTransform(*lut0, narrowedSymbol);

        // Compute position
        size_t index = 0;
        for (size_t j = ORDER; j > 0; --j) {
            index *= inverseLut0->size();
            index += lastSymbols[j];
        }
        index *= inverseLut0->size();
        index += lastSymbols[0];

        // Count
        ctr[index].second++;
        r.inc();
    }

    // Step through all single LUTs
    for (size_t i = 0; i < ctr.size(); i += inverseLut0->size()) {
        uint64_t counter = 0;
        for (auto it = ctr.begin() + i; it != ctr.begin() + i + inverseLut0->size(); ++it) {
            it->first = counter;
            counter++;
        }

        // Sort single LUT for frequency
        std::sort(ctr.begin() + i, ctr.begin() + i + inverseLut0->size(),
                  [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) {
                      if (a.second > b.second) {
                          return true;
                      }
                      if (a.second < b.second) {
                          return false;
                      }
                      return a.first < b.first;
                  });

        // Fill inverseLUT and write rank into second field
        counter = 0;
        bool placed = false;
        for (auto it = ctr.begin() + i; it != ctr.begin() + i + inverseLut0->size(); ++it) {
            if (it->second == 0 && !placed) {
                placed = true;
            }
            if (!placed) {
                inverseLut1->emplace_back(it->first);
            } else {
                inverseLut1->emplace_back(0);
            }
            it->second = counter;
            counter++;
        }

        // Sort single LUT for symbol value
        std::sort(ctr.begin() + i, ctr.begin() + i + inverseLut0->size(),
                  [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) {
                      if (a.first < b.first) {
                          return true;
                      }
                      if (a.first > b.first) {
                          return false;
                      }
                      return a.second > b.second;
                  });

        // Use previously set second field to fill LUT
        for (auto it = ctr.begin() + i; it != ctr.begin() + i + inverseLut0->size(); ++it) {
            lut1->emplace_back(it->second);
        }
    }
}

void transformLutTransform(unsigned order, DataBlock* const transformedSymbols, DataBlock* const inverseLUT,
                           DataBlock* const inverseLUT1) {
    std::vector<std::pair<uint64_t, uint64_t>> lut;
    DataBlock fastlut(0,
                      transformedSymbols->getWordSize());  // For small, dense symbol spaces
    DataBlock lut1(0, transformedSymbols->getWordSize());
    inferLut(order, *transformedSymbols, &lut, &fastlut, inverseLUT, &lut1, inverseLUT1);
    if (lut.empty()) {
        inverseLUT->clear();
        inverseLUT1->clear();
        return;
    }
    transformLutTransform_core(order, lut, fastlut, lut1, transformedSymbols);
}

void inverseTransformLutTransform(unsigned order, DataBlock* const symbols, DataBlock* const inverseLUT,
                                  DataBlock* const inverseLUT1) {
    inverseTransformLutTransform_core(order, symbols, inverseLUT, inverseLUT1);
}

}  // namespace gabac

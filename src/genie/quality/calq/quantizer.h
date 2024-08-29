/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

class Quantizer {
 public:
    Quantizer();
    explicit Quantizer(const std::map<int, int>& inverseLut);
    virtual ~Quantizer();

    [[nodiscard]] int valueToIndex(const int& value) const;
    [[nodiscard]] int indexToReconstructionValue(const int& index) const;
    [[nodiscard]] int valueToReconstructionValue(const int& value) const;

    [[nodiscard]] const std::map<int, int>& inverseLut() const;

    void print() const;

 protected:
    // value->(index,reconstructionValue)
    std::map<int, std::pair<int, int>> lut_;
    std::map<int, int> inverseLut_;  // index->reconstructionValue
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

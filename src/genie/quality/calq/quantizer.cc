/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/quantizer.h"

// -----------------------------------------------------------------------------

#include <map>
#include <sstream>
#include <string>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/error_exception_reporter.h"
#include "genie/quality/calq/log.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

Quantizer::Quantizer() : lut_(), inverseLut_() {}

// -----------------------------------------------------------------------------

Quantizer::Quantizer(const std::map<int, int>& inverseLut) : lut_(), inverseLut_(inverseLut) {}

// -----------------------------------------------------------------------------

Quantizer::~Quantizer() = default;

// -----------------------------------------------------------------------------

int Quantizer::valueToIndex(const int& value) const {
    if (lut_.find(value) == lut_.end()) {
        throwErrorException("Value out of range");
    }
    return lut_.at(value).first;
}

// -----------------------------------------------------------------------------

int Quantizer::indexToReconstructionValue(const int& index) const {
    if (inverseLut_.find(index) == inverseLut_.end()) {
        throwErrorException("Index not found");
    }
    return inverseLut_.at(index);
}

// -----------------------------------------------------------------------------

int Quantizer::valueToReconstructionValue(const int& value) const {
    if (lut_.find(value) == lut_.end()) {
        throwErrorException("Value out of range");
    }

    return lut_.at(value).second;
}

// -----------------------------------------------------------------------------

const std::map<int, int>& Quantizer::inverseLut() const { return inverseLut_; }

// -----------------------------------------------------------------------------

void Quantizer::print() const {
    std::stringstream stream;
    stream << "LUT:" << std::endl;
    for (auto const& lutElem : lut_) {
        stream << "  " << lutElem.first << ": ";
        stream << lutElem.second.first << ",";
        stream << lutElem.second.second << std::endl;
    }

    stream << "Inverse LUT:" << std::endl;
    for (auto const& inverseLutElem : inverseLut_) {
        stream << "  " << inverseLutElem.first << ": ";
        stream << inverseLutElem.second << std::endl;
    }

    std::string line;
    while (std::getline(stream, line)) {
        getLogging().standardOut(line);
    }
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

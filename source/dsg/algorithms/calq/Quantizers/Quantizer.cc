/** @file Quantizer.cc
 *  @brief This file contains the implementation of the Quantizer base class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "QualCodec/Quantizers/Quantizer.h"

#include "Common/Exceptions.h"

namespace calq {

Quantizer::Quantizer(void) : lut_() , inverseLut_() {}

Quantizer::Quantizer(const std::map<int, int> &inverseLut)
    : lut_(),
      inverseLut_(inverseLut) {}

Quantizer::~Quantizer(void) {}

int Quantizer::valueToIndex(const int &value) const {
    if (lut_.find(value) == lut_.end()) {
        throwErrorException("Value out of range");
    }
    return lut_.at(value).first;
}

int Quantizer::indexToReconstructionValue(const int &index) const {
    if (inverseLut_.find(index) == inverseLut_.end()) {
        throwErrorException("Index not found");
    }
    return inverseLut_.at(index);
}

int Quantizer::valueToReconstructionValue(const int &value) const {
    if (lut_.find(value) == lut_.end()) {
        throwErrorException("Value out of range");
    }

    return lut_.at(value).second;
}

const std::map<int, int> & Quantizer::inverseLut(void) const {
    return inverseLut_;
}

void Quantizer::print(void) const {
    std::cout << "LUT:" << std::endl;
    for (auto const &lutElem : lut_) {
        std::cout << "  " << lutElem.first << ": ";
        std::cout << lutElem.second.first << ",";
        std::cout << lutElem.second.second << std::endl;
    }

    std::cout << "Inverse LUT:" << std::endl;
    for (auto const &inverseLutElem : inverseLut_) {
        std::cout << "  " << inverseLutElem.first << ": ";
        std::cout << inverseLutElem.second << std::endl;
    }
}

}  // namespace calq


/** @file SAMPileupDeque.cc
 *  @brief This file contains the implementation of the SAMPileupDeque class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "IO/SAM/SAMPileupDeque.h"

#include "Common/Exceptions.h"

namespace calq {

SAMPileupDeque::SAMPileupDeque(void) : pileups_() , posMax_(0) , posMin_(0) {}

SAMPileupDeque::~SAMPileupDeque(void) {}

const SAMPileup& SAMPileupDeque::back(void) const {
    if (pileups_.empty() == true) {
        throwErrorException("Deque is empty");
    }
    return pileups_.back();
}

void SAMPileupDeque::clear(void) {
    pileups_.clear();
    posMax_ = 0;
    posMin_ = 0;
}

bool SAMPileupDeque::empty(void) const {
    if (pileups_.empty() == true)
        return true;
    return false;
}

const SAMPileup & SAMPileupDeque::front(void) const {
    if (pileups_.empty() == true) {
        throwErrorException("Deque is empty");
    }
    return pileups_.front();
}

size_t SAMPileupDeque::length(void) const {
    return posMax_ - posMin_ + 1;
}

const SAMPileup & SAMPileupDeque::operator[](const size_t &n) const {
    return pileups_.at(n);
}

void SAMPileupDeque::pop_back(void) {
    if (pileups_.empty() == true) {
        throwErrorException("Deque is empty");
    }
    pileups_.pop_back();
    posMax_--;
}

void SAMPileupDeque::pop_front(void) {
    if (pileups_.empty() == true) {
        throwErrorException("Deque is empty");
    }
    pileups_.pop_front();
    posMin_++;
}

size_t SAMPileupDeque::size(void) const {
    return pileups_.size();
}

void SAMPileupDeque::print(void) const {
    if (pileups_.empty() == true) {
        throwErrorException("Deque is empty");
    }

    for (auto const &samPileup : pileups_) {
        samPileup.print();
    }
}

uint32_t SAMPileupDeque::posMax(void) const {
    return posMax_;
}

uint32_t SAMPileupDeque::posMin(void) const {
    return posMin_;
}

void SAMPileupDeque::setPosMax(const uint32_t &posMax) {
    if (posMax < posMax_) {
        throwErrorException("posMax range");
    }
    posMax_ = posMax;
    pileups_.resize(length());
}

void SAMPileupDeque::setPosMin(const uint32_t &posMin) {
    if (posMin < posMin_) {
        throwErrorException("posMin range");
    }

    if (empty() == true) {
        posMin_ = posMin;
    } else {
        for (uint32_t i = posMin_; i < posMin ; i++) { pop_front(); }
    }
}

}  // namespace calq


/** @file Exceptions.cc
 *  @brief This file contains the implementations of the exception classes
 *         defined in Exceptions.h.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "Common/Exceptions.h"

namespace calq {

Exception::Exception(const std::string &msg) : msg_(msg) {}

Exception::~Exception(void) throw() {}

std::string Exception::getMessage(void) const {
    return msg_;
}

const char * Exception::what(void) const throw() {
    return msg_.c_str();
}

}  // namespace calq


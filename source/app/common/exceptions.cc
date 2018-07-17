// Copyright 2018 The genie authors


/**
 *  @file exceptions.cc
 *  @brief Exceptions implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "exceptions.h"


namespace dsg {
namespace common {


Exception::Exception(
    const std::string& message)
    : m_message(message)
{
    // Nothing to do here.
}


Exception::~Exception(void) throw()
{
    // Nothing to do here.
}


std::string Exception::getMessage(void) const
{
    return m_message;
}


const char * Exception::what(void) const throw()
{
    return m_message.c_str();
}


RuntimeErrorException::RuntimeErrorException(
    const std::string& message)
    : Exception(message)
{
    // Nothing to do here.
}


RuntimeErrorReporter::RuntimeErrorReporter(
    const std::string& file,
    const std::string& function,
    const int line)
    : m_file(file)
    , m_function(function)
    , m_line(line)
{
    // Nothing to do here.
}


}  // namespace common
}  // namespace dsg


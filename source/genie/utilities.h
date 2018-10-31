// Copyright 2018 The genie authors


/**
 *  @file utilities.h
 *  @brief Utilities interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_COMMON_UTILITIES_H_
#define DSG_COMMON_UTILITIES_H_


#include <string>


namespace dsg {
namespace common {


std::string dateTime(void);


std::string fileBaseName(
    const std::string& path);


bool fileExists(
    const std::string &path);


}  // namespace common
}  // namespace dsg


#endif  // DSG_COMMON_UTILITIES_H_

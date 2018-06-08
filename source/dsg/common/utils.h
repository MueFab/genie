// Copyright 2018 The genie authors


/**
 *  @file utils.h
 *  @brief Utilities interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_COMMON_UTILS_H_
#define DSG_COMMON_UTILS_H_


#include <string>


namespace dsg {


std::string dateTime(void);

bool fileExists(const std::string &path);


}  // namespace dsg

#endif  // DSG_COMMON_UTILS_H_

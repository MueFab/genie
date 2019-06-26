/** @file helpers.h
 *  @brief This file contains generic helper functions.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#ifndef CALQ_COMMON_HELPERS_H_
#define CALQ_COMMON_HELPERS_H_

#include <string>

namespace calq {

std::string currentDateAndTime(void);
bool fileExists(const std::string &path);
std::string fileBaseName(const std::string &path);
std::string fileNameExtension(const std::string &path);
std::string removeFileNameExtension(const std::string &path);

}  // namespace calq

#endif  // CALQ_COMMON_HELPERS_H_


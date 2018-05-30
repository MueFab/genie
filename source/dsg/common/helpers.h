// Copyright 2018 The genie authors


/**
 *  @file helpers.h
 *  @brief Utility functions interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_COMMON_HELPERS_H_
#define DSG_COMMON_HELPERS_H_


#include <stdio.h>


namespace dsg {


/**
 *  @brief Print the current date and time to stream in ISO 8601 format
 *  @param stream Stream (usually stdout or stderr) where to print to
 *  @return void.
 */
void printCurrentDateAndTime(FILE *stream);


}  // namespace dsg


#endif  // DSG_COMMON_HELPERS_H_

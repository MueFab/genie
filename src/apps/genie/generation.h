// Copyright 2018 The genie authors

/**
 *  @file generation.h
 *  @brief Main entry point for descriptor stream generation algorithms
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_GENERATION_H_
#define DSG_GENERATION_H_

#include <map>
#include <vector>
#include "program-options.h"

namespace dsg {

void generation(const ProgramOptions& programOptions);

void decompression(const ProgramOptions& programOptions);

}  // namespace dsg

#endif  // DSG_GENERATION_H_

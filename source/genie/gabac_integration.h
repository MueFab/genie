// Copyright 2018 The genie authors

#ifndef GENIE_GABAC_INTEGRATION_H
#define GENIE_GABAC_INTEGRATION_H

#include <string>
#include <vector>

namespace dsg {

// Compress/Decompress a single file
void compress_one_file(const std::string& file, bool decompress);

// Decompress a list of files in parallel
void run_gabac(const std::vector<std::string>& files, bool decompress);
}
#endif //GENIE_GABAC_INTEGRATION_H

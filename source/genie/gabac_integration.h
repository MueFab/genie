// Copyright 2018 The genie authors

#ifndef GENIE_GABAC_INTEGRATION_H
#define GENIE_GABAC_INTEGRATION_H

#include <map>
#include <string>
#include <vector>
#include <limits>

namespace dsg {

// Compress/Decompress a single file
void compress_one_file(const std::string& file, const std::string& config, bool decompress);

// Decompress a list of files in parallel
void run_gabac(const std::vector<std::string>& files, const std::string& config, bool decompress, size_t threads);

struct gabac_stream_params {
    uint64_t maxval;
    uint8_t wordsize;
};

inline const std::map<std::string, gabac_stream_params>& getSPRINGParams() {
    static const std::map<std::string, gabac_stream_params> mapping = {
            {"S0.0", {1022, 8}},
            {"S1.0", {1, 8}},
            {"S3.0", {1, 8}},
            {"S3.1", {511, 8}},
            {"S4.0", {1, 8}},
            {"S4.1", {4, 8}},
            {"S6.0", {4, 8}},
            {"S7.0", {std::numeric_limits<uint32_t >::max(), 8}},
            {"S8.0", {8, 8}},
            {"S8.1", {65535, 8}},
            {"S8.2", {512000, 8}},
            {"S8.3", {512000, 8}},
            {"S8.4", {std::numeric_limits<uint32_t >::max(), 8}},
            {"S8.5", {std::numeric_limits<uint32_t >::max(), 8}},
            {"S8.6", {512000, 8}},
            {"S8.7", {512000, 8}},
            {"S12.0", {5, 8}},
            {"Q", {127, 1}},
            {"T0", {9, 8}},
            {"T1", {1, 8}},
            {"T2", {127, 8}},
            {"T3", {127, 8}},
            {"T4", {std::numeric_limits<uint32_t >::max(), 8}},
            {"T5", {255, 8}}

    };
    return mapping;
}

}
#endif //GENIE_GABAC_INTEGRATION_H

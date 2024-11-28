//
// Created by Irvan on 19.11.2024.
//

#ifndef COMMONS_H
#define COMMONS_H

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#include <cassert>

namespace genie::entropy::rans {

// ---------------------------------------------------------------------------------------------------------------------

void panic(const char* fmt, ...);

// ---------------------------------------------------------------------------------------------------------------------

uint8_t* read_file(const char* filename, size_t* out_size);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Symbol table and related information.
 */
struct SymbolStats {
    uint32_t freqs[256];
    uint32_t cum_freqs[257];

    void count_freqs(uint8_t const* in, size_t nbytes);
    void calc_cum_freqs();
    void normalize_freqs(uint32_t target_total);
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief RANS Parameter struct containing symbol table and other information.
 */
struct RANSParams {
    SymbolStats stats;
    uint32_t num_symbols;
    uint32_t compressed_size;
};

} // namespace genie::entropy::rans

#endif //COMMONS_H

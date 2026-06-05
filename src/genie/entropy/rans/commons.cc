#include "commons.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::rans {

void panic(const char *fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    fputs("\n", stderr);

    exit(1);
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t* read_file(char const* filename, size_t* out_size)
{
    FILE* f = fopen(filename, "rb");
    if (!f)
        panic("file not found: %s\n", filename);

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buf = new uint8_t[size];
    if (fread(buf, size, 1, f) != 1)
        panic("read failed\n");

    fclose(f);
    if (out_size)
        *out_size = size;

    return buf;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---- Stats

void SymbolStats::count_freqs(uint8_t const* in, size_t nbytes)
{
    for (int idx_i=0; idx_i < 256; idx_i++)
        freqs[idx_i] = 0;

    for (size_t idx_i=0; idx_i < nbytes; idx_i++)
        freqs[in[idx_i]]++;
}

void SymbolStats::calc_cum_freqs()
{
    cum_freqs[0] = 0;
    for (int idx_i=0; idx_i < 256; idx_i++)
        cum_freqs[idx_i+1] = cum_freqs[idx_i] + freqs[idx_i];
}

// ---------------------------------------------------------------------------------------------------------------------

void SymbolStats::normalize_freqs(uint32_t target_total)
{
    assert(target_total >= 256);

    calc_cum_freqs();
    uint32_t cur_total = cum_freqs[256];

    // resample distribution based on cumulative freqs
    for (int idx_i = 1; idx_i <= 256; idx_i++)
        cum_freqs[idx_i] = ((uint64_t)target_total * cum_freqs[idx_i])/cur_total;

    // if we nuked any non-0 frequency symbol to 0, we need to steal
    // the range to make the frequency nonzero from elsewhere.
    for (int idx_i=0; idx_i < 256; idx_i++) {
        if (freqs[idx_i] && cum_freqs[idx_i+1] == cum_freqs[idx_i]) {
            // symbol idx_i was set to zero freq

            // find best symbol to steal frequency from (try to steal from low-freq ones)
            uint32_t best_freq = ~0u;
            int best_steal = -1;
            for (int idx_j=0; idx_j < 256; idx_j++) {
                uint32_t freq = cum_freqs[idx_j+1] - cum_freqs[idx_j];
                if (freq > 1 && freq < best_freq) {
                    best_freq = freq;
                    best_steal = idx_j;
                }
            }
            assert(best_steal != -1);

            // and steal from it!
            if (best_steal < idx_i) {
                for (int idx_j = best_steal + 1; idx_j <= idx_i; idx_j++)
                    cum_freqs[idx_j]--;
            } else {
                assert(best_steal > idx_i);
                for (int idx_j = idx_i + 1; idx_j <= best_steal; idx_j++)
                    cum_freqs[idx_j]++;
            }
        }
    }

    // calculate updated freqs and make sure we didn't screw anything up
    assert(cum_freqs[0] == 0 && cum_freqs[256] == target_total);
    for (int idx_i=0; idx_i < 256; idx_i++) {
        if (freqs[idx_i] == 0)
            assert(cum_freqs[idx_i+1] == cum_freqs[idx_i]);
        else
            assert(cum_freqs[idx_i+1] > cum_freqs[idx_i]);

        // calc updated freq
        freqs[idx_i] = cum_freqs[idx_i+1] - cum_freqs[idx_i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}

#include "decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace rans {
// ---------------------------------------------------------------------------------------------------------------------

RANSDecoder::RANSDecoder(){};

void RANSDecoder::decode(std::istream &input, std::ostream &output) {
    uint32_t compressed_size = 0;

    RANSParams param;

    // read frequencies
    input.read(reinterpret_cast<char*>(param.stats.freqs), sizeof(param.stats.freqs));
    param.stats.calc_cum_freqs();

    // read the num of symbols
    input.read(reinterpret_cast<char*>(&param.num_symbols), sizeof(param.num_symbols));
    input.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));

    // read compressed data
    std::vector<uint32_t> compressed_data(compressed_size);
    input.read(reinterpret_cast<char*>(compressed_data.data()), compressed_size * sizeof(uint32_t));

    Rans64DecSymbol dsyms[256];
    for (int i = 0; i < 256; ++i) {
        Rans64DecSymbolInit(&dsyms[i], param.stats.cum_freqs[i], param.stats.freqs[i]);
    }

    std::vector<uint8_t> cum2sym(param.stats.cum_freqs[256], 0);
    for (int s = 0; s < 256; ++s) {
        for (uint32_t i = param.stats.cum_freqs[s]; i < param.stats.cum_freqs[s + 1]; ++i) {
            cum2sym[i] = static_cast<uint8_t>(s);
        }
    }

    // decode compressed dta
    Rans64State rans;
    uint32_t* ptr = compressed_data.data();
    Rans64DecInit(&rans, &ptr);

    size_t out_size = static_cast<size_t>(param.num_symbols);
    std::vector<uint8_t> dec_bytes(out_size, 0);

    for (size_t i = 0; i < out_size; ++i) {
        uint32_t symbol_code = Rans64DecGet(&rans, 14);
        uint8_t symbol = cum2sym[symbol_code];
        dec_bytes[i] = symbol;
        Rans64DecAdvanceSymbol(&rans, &ptr, &dsyms[symbol], 14);
    }

    output.write(reinterpret_cast<char*>(dec_bytes.data()), dec_bytes.size());
}

// ---------------------------------------------------------------------------------------------------------------------
}
}
}


#include "decoder.h"
#include <genie/util/runtime-exception.h>
#include <cstdint>

namespace genie::entropy::rans {

RANSDecoder::RANSDecoder(){};

void RANSDecoder::decode(std::istream& input, std::ostream& output, uint32_t num_interleavings) {
    if (num_interleavings != 1 && num_interleavings != 2) throw std::runtime_error("num_interleavings must be 1 or 2.");

    RANSParams param;
    uint8_t flag_byte;
    input.read(reinterpret_cast<char*>(&flag_byte), sizeof(flag_byte));
    if (!input) throw std::runtime_error("Failed to read is_table_stored flag.");
    bool is_table_stored = (flag_byte != 0);

    input.read(reinterpret_cast<char*>(param.stats.freqs), sizeof(param.stats.freqs));
    if (!input) throw std::runtime_error("Failed to read frequency table.");

    std::vector<uint8_t> cum2sym;
    if (is_table_stored) {
        input.read(reinterpret_cast<char*>(param.stats.cum_freqs), sizeof(param.stats.cum_freqs));
        if (!input) throw std::runtime_error("Failed to read cum_freqs.");
        cum2sym.resize(param.stats.cum_freqs[256]);
        input.read(reinterpret_cast<char*>(cum2sym.data()), cum2sym.size());
        if (!input) throw std::runtime_error("Failed to read cum2sym.");
    } else {
        param.stats.calc_cum_freqs();
        cum2sym.resize(param.stats.cum_freqs[256]);
        for (int s = 0; s < 256; ++s)
            for (uint32_t i = param.stats.cum_freqs[s]; i < param.stats.cum_freqs[s+1]; ++i)
                cum2sym[i] = static_cast<uint8_t>(s);
    }

    input.read(reinterpret_cast<char*>(&param.num_symbols), sizeof(param.num_symbols));
    if (!input) throw std::runtime_error("Failed to read num_symbols.");
    input.read(reinterpret_cast<char*>(&param.compressed_size), sizeof(param.compressed_size));
    if (!input) throw std::runtime_error("Failed to read compressed_size.");

    std::vector<uint32_t> compressed_data(param.compressed_size);
    input.read(reinterpret_cast<char*>(compressed_data.data()), param.compressed_size * sizeof(uint32_t));
    if (!input) throw std::runtime_error("Failed to read compressed data.");

    Rans64DecSymbol dsyms[256];
    for (int i = 0; i < 256; ++i) Rans64DecSymbolInit(&dsyms[i], param.stats.cum_freqs[i], param.stats.freqs[i]);

    std::vector<uint8_t> dec_bytes(param.num_symbols, 0);
    std::vector<Rans64State> rans_states(num_interleavings);
    uint32_t* ptr = compressed_data.data();
    for (auto& rans : rans_states) Rans64DecInit(&rans, &ptr);

    for (size_t symbol_idx = 0; symbol_idx < param.num_symbols;) {
        for (size_t j = 0; j < num_interleavings && symbol_idx < param.num_symbols; ++j) {
            uint32_t symbol_code = Rans64DecGet(&rans_states[j], PROB_BITS);
            uint8_t symbol = cum2sym[symbol_code];
            dec_bytes[symbol_idx++] = symbol;
            Rans64DecAdvanceSymbol(&rans_states[j], &ptr, &dsyms[symbol], PROB_BITS);
        }
    }

    output.write(reinterpret_cast<char*>(dec_bytes.data()), dec_bytes.size());
}

} // namespace genie::entropy::rans

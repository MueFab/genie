#include "encoder.h"
#include <genie/util/runtime_exception.h>
#include <cstdint>

namespace genie::entropy::rans {

void RANSEncoder::encode(std::istream& input, std::ostream& output, bool is_table_stored, uint32_t num_interleavings) {
    if (num_interleavings != 1 && num_interleavings != 2) throw std::runtime_error("num_interleavings must be 1 or 2.");

    std::vector<uint8_t> in_bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    RANSParams param;
    param.stats.count_freqs(in_bytes.data(), in_bytes.size());
    param.stats.normalize_freqs(PROB_SCALE);
    param.num_symbols = static_cast<uint32_t>(in_bytes.size());

    Rans64EncSymbol esyms[256];
    for (int i = 0; i < 256; ++i) Rans64EncSymbolInit(&esyms[i], param.stats.cum_freqs[i], param.stats.freqs[i], PROB_BITS);

    uint8_t flag_byte = is_table_stored ? 1 : 0;
    output.write(reinterpret_cast<const char*>(&flag_byte), sizeof(flag_byte));
    output.write(reinterpret_cast<const char*>(param.stats.freqs), sizeof(param.stats.freqs));

    if (is_table_stored) {
        output.write(reinterpret_cast<const char*>(param.stats.cum_freqs), sizeof(param.stats.cum_freqs));
        std::vector<uint8_t> cum2sym(param.stats.cum_freqs[256]);
        for (int s = 0; s < 256; ++s)
            for (uint32_t i = param.stats.cum_freqs[s]; i < param.stats.cum_freqs[s + 1]; ++i)
                cum2sym[i] = static_cast<uint8_t>(s);
        output.write(reinterpret_cast<const char*>(cum2sym.data()), cum2sym.size());
    }

    std::vector<uint32_t> out_buf(in_bytes.size() * sizeof(uint32_t), 0);
    uint32_t* out_end = out_buf.data() + out_buf.size();
    std::vector<Rans64State> rans_states(num_interleavings);
    for (auto& rans : rans_states) Rans64EncInit(&rans);

    uint32_t* ptr = out_end;
    for (size_t i = in_bytes.size(); i > 0;)
        for (size_t j = 0; j < num_interleavings && i > 0; ++j)
            Rans64EncPutSymbol(&rans_states[j], &ptr, &esyms[in_bytes[--i]], PROB_BITS);

    for (auto& rans : rans_states) Rans64EncFlush(&rans, &ptr);

    param.compressed_size = static_cast<uint32_t>(out_end - ptr);
    output.write(reinterpret_cast<const char*>(&param.num_symbols), sizeof(param.num_symbols));
    output.write(reinterpret_cast<const char*>(&param.compressed_size), sizeof(param.compressed_size));
    output.write(reinterpret_cast<const char*>(ptr), param.compressed_size * sizeof(uint32_t));
}

} // namespace genie::entropy::rans

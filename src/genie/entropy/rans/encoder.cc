#include "encoder.h"
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace rans {

// ---------------------------------------------------------------------------------------------------------------------

void RANSEncoder::encode(std::istream& input, std::ostream& output) {
    std::vector<uint8_t> in_bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    RANSParams param;
    param.stats.count_freqs(in_bytes.data(), in_bytes.size());
    param.stats.normalize_freqs(PROB_SCALE);
    param.num_symbols = static_cast<uint32_t>(in_bytes.size());

    // init the encoding symbols and
    Rans64EncSymbol esyms[256];
    for (int i = 0; i < 256; ++i) {
        Rans64EncSymbolInit(&esyms[i],
            param.stats.cum_freqs[i],
            param.stats.freqs[i],
            PROB_BITS);
    }

    // init buffer
    std::vector<uint32_t> out_buf(in_bytes.size() * sizeof(uint32_t), 0);
    uint32_t* out_end = out_buf.data() + out_buf.size();

    // init state
    Rans64State rans;
    Rans64EncInit(&rans);

    // encode
    uint32_t* ptr = out_end;
    for (size_t i = in_bytes.size(); i > 0; --i) { // Process in reverse
        uint8_t symbol = in_bytes[i - 1];
        Rans64EncPutSymbol(&rans, &ptr, &esyms[symbol], PROB_BITS);
    }
    Rans64EncFlush(&rans, &ptr);

    // get compressed size to write onto output stream
    param.compressed_size = out_end - ptr;
    auto compressed_size_u32 = static_cast<uint32_t>(param.compressed_size);

    // write frequency information, size of compressed data to later decompress, and the compressed data
    output.write(reinterpret_cast<char*>(param.stats.freqs), sizeof(param.stats.freqs));
    output.write(reinterpret_cast<char*>(&param.num_symbols), sizeof(param.num_symbols));
    output.write(reinterpret_cast<char*>(&compressed_size_u32), sizeof(compressed_size_u32));
    output.write(reinterpret_cast<char*>(ptr), param.compressed_size * sizeof(uint32_t));
}

// ---------------------------------------------------------------------------------------------------------------------
}
}
} // namespace genie::entropy::rans

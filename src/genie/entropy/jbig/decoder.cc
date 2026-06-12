/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include "jbig_interface.h"
#include <iostream>

namespace genie {
namespace entropy {
namespace jbig {

void JBIGDecoder::decode(
    std::stringstream& input,
    std::stringstream& output,
    uint32_t& ncols,
    uint32_t& nrows
) {
    const size_t srcLen = input.str().size();
    unsigned char* decompressedBuffer = nullptr;
    size_t dest_data_len = 0;

    unsigned long buf_nrows = 0, buf_ncols = 0;

    int ret = mpegg_jbig_decompress_default(
        &decompressedBuffer,
        &dest_data_len,
        (const unsigned char* )input.str().c_str(),
        srcLen,
        &buf_nrows,
        &buf_ncols
    );

    nrows = (uint32_t) buf_nrows;
    ncols = (uint32_t) buf_ncols;

    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    for (size_t idx_i = 0; idx_i < dest_data_len; ++idx_i) {
        output << decompressedBuffer[idx_i];
    }
    free(decompressedBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void JBIGDecoder::decode(
    const std::vector<uint8_t>& input,
    std::vector<uint8_t>& output,
    uint32_t& ncols,
    uint32_t& nrows
) {
    const size_t srcLen = input.size();
    if (srcLen == 0) return;

    unsigned char* decompressedBuffer = nullptr;
    size_t dest_data_len = 0;

    unsigned long buf_nrows = 0, buf_ncols = 0;

    int ret = mpegg_jbig_decompress_default(
        &decompressedBuffer,
        &dest_data_len,
        input.data(),
        srcLen,
        &buf_nrows,
        &buf_ncols
    );

    nrows = (uint32_t) buf_nrows;
    ncols = (uint32_t) buf_ncols;

    if (ret != 0) {
        std::cerr << "error with decompression\n";
    }
    output.clear();
    output.reserve(dest_data_len);
    for (size_t idx_i = 0; idx_i < dest_data_len; ++idx_i) {
        output.push_back(decompressedBuffer[idx_i]);
    }
    free(decompressedBuffer);
}

}  // namespace jbig
}  // namespace entropy
}  // namespace genie
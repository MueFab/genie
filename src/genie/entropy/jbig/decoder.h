/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_JBIG_DECODER_H
#define GENIE_ENTROPY_JBIG_DECODER_H

#include <sstream>
#include <vector>
#include <cstdint>

namespace genie {
namespace entropy {
namespace jbig {

class JBIGDecoder {
 public:
    JBIGDecoder() = default;

    void decode(std::stringstream& input, std::stringstream& output, uint32_t& ncols, uint32_t& nrows);

    void decode(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, uint32_t& ncols, uint32_t& nrows);
};

}  // namespace jbig
}  // namespace entropy
}  // namespace genie

#endif  // GENIE_ENTROPY_JBIG_DECODER_H

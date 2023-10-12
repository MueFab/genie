/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

void BinMatPayload::write(core::Writer& writer) const {
    if (codecID == core::AlgoID::JBIG) {
        for (auto byte : payload) writer.write(byte, 8);  //??
    } else if (codecID == core::AlgoID::CABAC) {
        writer.write(nrows, 32);
        writer.write(ncols, 32);
        for (auto byte : payload) writer.write(byte, 8);  //??
    }
}

void BinMatPayload::writeCompressed(core::Writer& writer) const {
    std::stringstream payloadStream;
    for (auto byte : payload) payloadStream.write((char*)&byte, 1);
    auto temp = payloadStream.str().size();
    if (temp == 0) {
    }
    std::stringstream compressedStream;
    if (codecID == core::AlgoID::JBIG) {
        genie::entropy::jbig::JBIGEncoder encoder;

        encoder.encode(payloadStream, compressedStream, ncols, nrows);
        for (auto byte : compressedStream.str()) writer.write(byte, 8);

    } else {
        writer.write(nrows, 4);
        writer.write(ncols, 4);
        writer.write(&payloadStream);
        for (auto byte : payloadStream.str()) writer.write(byte, 8);
    }
}

void RowColIdsPayload::write(core::Writer& writer) const {
    for (uint64_t i = 0; i < nelements; ++i) {
        writer.write(row_col_ids_elements[i], static_cast<uint8_t>(nbits_per_elem));
    }
}

void AmexPayload::write(core::Writer& writer) const {
    writer.write(nelems, 32);
    writer.write(nbits_per_elem, 8);
    for (uint32_t i = 0; i < nelements; ++i) {
        writer.write(is_one_flag[i], 1);
        if (is_one_flag[i]) writer.write(amax_elements[i], nbits_per_elem);
    }
}

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PAYLOAD_H
#define GENIE_GENOTYPE_PAYLOAD_H

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

class BinMatPayload {
 private:
    core::AlgoID codecID;
    std::vector<uint8_t> payload;  // JBIG or CABAC: ISO/IEC 11544 or ISO/IEC 23092-2
    uint32_t nrows;
    uint32_t ncols;
    std::vector<uint8_t> CompressedPayload;

 public:
    BinMatPayload(core::AlgoID _codecID, std::vector<uint8_t> _payload, uint32_t _nrows, uint32_t _ncols)
        : codecID(_codecID), payload(_payload), nrows(_nrows), ncols(_ncols), CompressedPayload{} {}
    size_t payloadSize() const { return payload.size(); }

    BinMatPayload(BinMatDtype binmat);
    BinMatPayload(BinMatDtype binmat, core::AlgoID _codecID);

    void write(core::Writer& writer) const;

    void writeCompressed(core::Writer& writer) const;
};

class RowColIdsPayload {
 private:
    uint64_t nelements;
    uint32_t nbits_per_elem;
    std::vector<uint64_t> row_col_ids_elements;
    uint32_t sizeInBytes;

 public:
    RowColIdsPayload(uint64_t _nelements, uint32_t _nbits_per_elem, std::vector<uint64_t> _row_col_ids_elements)
        : nelements(_nelements),
          nbits_per_elem(_nbits_per_elem),
          row_col_ids_elements(_row_col_ids_elements),
          sizeInBytes(static_cast<uint32_t>(_nbits_per_elem * _nelements + 7) / 8) {}
    size_t payloadSize() const { return row_col_ids_elements.size(); }

    uint32_t GetSize() const { return sizeInBytes; }
    void Write(core::Writer& writer) const;
};

class AmaxPayload {
 private:
    uint32_t nelems;
    uint8_t nbits_per_elem;
    uint32_t nelements;
    std::vector<uint64_t> amax_elements;

 public:
    AmaxPayload(uint32_t _nelems, uint8_t _nbits_per_elem, uint32_t _nelements, std::vector<uint64_t> _amax_elements)
        : nelems(_nelems), nbits_per_elem(_nbits_per_elem), nelements(_nelements), amax_elements(_amax_elements) {}

    uint32_t GetSize() const {
        uint32_t sizeInBits = 32 + 4;  // size of nelems + size of nbits_per_elem
        for (uint32_t i = 0; i < nelements; ++i) {
            sizeInBits++;  // size of is_one_flag
            if (amax_elements[i] != 0) {
                sizeInBits += nbits_per_elem;  // nbits_per_entry == nbits_per_elem
            }
        }
        return static_cast<uint32_t>((sizeInBits + 7) / 8);
    }

    void Write(core::Writer& writer) const;
};
// ---------------------------------------------------------------------------------------------------------------------

class GenotypePayload {
 private:
    GenotypeParameters genotypeParameters;

    std::vector<BinMatPayload> variants_payload;  // bin_mat_payload 6.4.4.3.2.2

    BinMatPayload phases_payload;  // bin_mat_payload 6.4.4.3.2.2

    std::vector<RowColIdsPayload> sort_variants_row_ids_payload;  // row_col_ids_payload 6.4.4.3.2.3
    std::vector<RowColIdsPayload> sort_variants_col_ids_payload;  // row_col_ids_payload 6.4.4.3.2.3

    uint32_t variants_amax_payload_size;
    AmaxPayload variants_amax_payload;  // amax_payload 6.4.4.3.2.4

 public:
    GenotypePayload(GenotypeParameters& _genotypeParameters, std::vector<BinMatPayload> _variants_payload,
                    BinMatPayload _phases_payload, std::vector<RowColIdsPayload> _sort_variants_row_ids_payload,
                    std::vector<RowColIdsPayload> _sort_variants_col_ids_payload, AmaxPayload _variants_amax_payload)
        : genotypeParameters(_genotypeParameters),
          variants_payload(_variants_payload),
          phases_payload(_phases_payload),
          sort_variants_row_ids_payload(_sort_variants_row_ids_payload),
          sort_variants_col_ids_payload(_sort_variants_col_ids_payload),
          variants_amax_payload(_variants_amax_payload) {}

    GenotypePayload(genie::genotype::EncodingBlock& dataBlock,
                    genie::genotype::GenotypeParameters& genotpyParameters);  // build from datablock

    void Write(core::Writer& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

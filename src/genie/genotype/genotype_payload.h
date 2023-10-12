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
#include "genie/entropy/jbig/encoder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

struct VariantsPayload {
    uint32_t variants_payload_size;
    std::vector<uint8_t> variants_payload;
    uint32_t sort_variants_row_ids_payload_size;
    size_t payloadSize() const { return variants_payload.size(); }
};

// ---------------------------------------------------------------------------------------------------------------------

struct PhasesPayload {
    bool sort_variants_rows_flag;
    bool sort_variants_cols_flag;
    bool transpose_variants_mat_flag;
    genie::core::AlgoID variants_codec_ID;
};

class BinMatPayload {
 private:
    core::AlgoID codecID;
    std::vector<uint8_t> payload;  // JBIG or CABAC: ISO/IEC 11544 or ISO/IEC 23092-2
    uint32_t nrows;
    uint32_t ncols;
    std::vector<uint8_t> CompressedPayload;

 public:
    BinMatPayload(core::AlgoID _codecID, std::vector<uint8_t> _payload, uint32_t _nrows, uint32_t _ncols)
        : codecID(_codecID), payload(_payload), nrows(_nrows), ncols(_ncols) {}
    size_t payloadSize() const { return payload.size(); 
}

    void write(core::Writer& writer) const;

    void writeCompressed(core::Writer& writer) const;
};

class RowColIdsPayload {
 private:
    uint64_t nelements;
    uint64_t nbits_per_elem;
    std::vector<uint64_t> row_col_ids_elements;

 public:
    RowColIdsPayload(uint64_t _nelements, uint64_t _nbits_per_elem, std::vector<uint64_t> _row_col_ids_elements)
        : nelements(_nelements), nbits_per_elem(_nbits_per_elem), row_col_ids_elements(_row_col_ids_elements) {}
    size_t payloadSize() const { return row_col_ids_elements.size(); }

    void write(core::Writer& writer) const;
};

class AmexPayload {
 private:
    uint32_t nelems;
    uint8_t nbits_per_elem;
    uint32_t nelements;
    std::vector<bool> is_one_flag;
    std::vector<uint64_t> amax_elements;

 public:
    AmexPayload(uint32_t _nelems, uint8_t _nbits_per_elem, uint32_t _nelements, std::vector<bool> _is_one_flag,
                std::vector<uint64_t> _amax_elements)
        : nelems(_nelems), nbits_per_elem(_nbits_per_elem), nelements(_nelements), amax_elements(_amax_elements) {}

    size_t sizeInBytes() const { return (nelems * nbits_per_elem + 7) / 8; }

    void write(core::Writer& writer) const;
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
    AmexPayload variants_amax_payload;  // amax_payload 6.4.4.3.2.4

 public:
    GenotypePayload(GenotypeParameters _genotypeParameters, std::vector<BinMatPayload> _variants_payload,
                    BinMatPayload _phases_payload, std::vector<RowColIdsPayload> _sort_variants_row_ids_payload,
                    std::vector<RowColIdsPayload> _sort_variants_col_ids_payload, AmexPayload _variants_amax_payload)
        : genotypeParameters(_genotypeParameters),
          variants_payload(_variants_payload),
          phases_payload(_phases_payload),
          sort_variants_row_ids_payload(_sort_variants_row_ids_payload),
          sort_variants_col_ids_payload(_sort_variants_col_ids_payload),
          variants_amax_payload(_variants_amax_payload) {}

    void write(core::Writer& writer) const

    {
        uint8_t num_variants_payloads = genotypeParameters.getBinarizationID() == BinarizationID::BIT_PLANE &&
                                                genotypeParameters.isConcatenated() == ConcatAxis::DO_NOT_CONCAT
                                            ? genotypeParameters.getNumBitPlanes()
                                            : 1;
        for (auto i = 0; i < num_variants_payloads; ++i) {
            std::stringstream tempstream;
            core::Writer writesize(&tempstream);
            variants_payload[i].writeCompressed(writesize);
            writer.write(tempstream.str().size(), 32);
            variants_payload[i].writeCompressed(writer);
            auto variantsPayloadsParams = genotypeParameters.getVariantsPayloadParams();
            auto indecRowIds = 0;
            if (variantsPayloadsParams[i].sort_variants_rows_flag) {
                writer.write(sort_variants_row_ids_payload[indecRowIds].payloadSize(), 32);
                sort_variants_row_ids_payload[indecRowIds].write(writer);
                ++indecRowIds;
            }
            auto indecColIds = 0;
            if (variantsPayloadsParams[i].sort_variants_cols_flag) {
                writer.write(sort_variants_col_ids_payload[indecColIds].payloadSize(), 32);
                sort_variants_col_ids_payload[indecColIds].write(writer);
                ++indecColIds;
            }
            if (genotypeParameters.getBinarizationID() == BinarizationID::ROW_BIN) {
                writer.write(variants_amax_payload.sizeInBytes(), 32);
            }
        }
        auto written = writer.getBitsWritten();
        std::cerr << "genotype payload: " << written << std::endl;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

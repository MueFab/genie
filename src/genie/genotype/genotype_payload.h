/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PAYLOAD_H
#define GENIE_GENOTYPE_PAYLOAD_H

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <iostream>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/entropy/jbig/encoder.h"


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
    BinMatPayload(core::AlgoID codecID, std::vector<uint8_t> payload, uint32_t nrows = 0, uint32_t ncols = 0)
        : codecID(codecID), payload(payload), nrows(nrows), ncols(ncols) {}
    size_t payloadSize() const { return payload.size(); }

    void write(core::Writer writer) {
        if (codecID == core::AlgoID::JBIG) {
            for (auto byte : payload) writer.write(byte, 8);  //??
        } else if (codecID == core::AlgoID::CABAC) {
            writer.write(nrows, 32);
            writer.write(ncols, 32);
            for (auto byte : payload) writer.write(byte, 8);  //??
        }
    }
    void writeCompressed(core::Writer writer) {
        std::stringstream compressedStream;
        std::stringstream payloadStream;
        core::Writer writeCompressed(&compressedStream);
            for (auto byte : payload) payloadStream.write((char*)&byte, 1);
        if (codecID == core::AlgoID::JBIG) {
            genie::entropy::jbig::JBIGEncoder encoder;
            encoder.encode(payloadStream, compressedStream,ncols,nrows);
        } else {
            payloadStream.write((char*)&nrows, 4);
            payloadStream.write((char*)&ncols, 4);
            //genie::entropy::cabac::CABACEncoder encoder;
            //encoder.encode(payloadStream, compressedStream, ncols, nrows);
        }
        write(writeCompressed);
    }
};

class RowColIdsPayload {
 private:
    uint64_t nelements;
    uint64_t nbits_per_elem;
    std::vector<uint64_t> row_col_ids_elements;

 public:
    RowColIdsPayload(uint64_t nelements, uint64_t nbits_per_elem, std::vector<uint64_t> row_col_ids_elements)
        : nelements(nelements), nbits_per_elem(nbits_per_elem), row_col_ids_elements(row_col_ids_elements) {}
    size_t payloadSize() const { return row_col_ids_elements.size(); }

    void write(core::Writer writer) {
        for (auto i = 0; i < nelements; ++i) {
            writer.write(row_col_ids_elements[i], static_cast<uint8_t>(nbits_per_elem));
        }
    }
};

class AmexPayload {
 private:
    uint32_t nelems;
    uint8_t nbits_per_elem;
    uint32_t nelements;
    std::vector<bool> is_one_flag;
    std::vector<uint64_t> amax_elements;

 public:
    AmexPayload(uint32_t nelems, uint8_t nbits_per_elem, uint32_t nelements, std::vector<bool> is_one_flag,
                std::vector<uint64_t> amax_elements)
        : nelems(nelems), nbits_per_elem(nbits_per_elem), nelements(nelements), amax_elements(amax_elements) {}

    size_t sizeInBytes() { return (nelems * nbits_per_elem + 7) / 8; }

    void write(core::Writer writer) {
        writer.write(nelems, 32);
        writer.write(nbits_per_elem, 8);
        for (uint32_t i = 0; i < nelements; ++i) {
            writer.write(is_one_flag[i], 1);
            if (is_one_flag[i]) writer.write(amax_elements[i], nbits_per_elem);
        }
    }
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
    GenotypePayload(GenotypeParameters genotypeParameters, std::vector<BinMatPayload> variants_payload,
                    BinMatPayload phases_payload, std::vector<RowColIdsPayload> sort_variants_row_ids_payload,
                    std::vector<RowColIdsPayload> sort_variants_col_ids_payload, AmexPayload variants_amax_payload)
        : genotypeParameters(genotypeParameters),
          variants_payload(variants_payload),
          phases_payload(phases_payload),
          sort_variants_row_ids_payload(sort_variants_row_ids_payload),
          sort_variants_col_ids_payload(sort_variants_col_ids_payload),
          variants_amax_payload(variants_amax_payload) {}

    void write(core::Writer writer) {
        uint8_t num_variants_payloads = genotypeParameters.getBinarizationID() == BinarizationID::BIT_PLANE &&
                                                genotypeParameters.isConcatenated() == ConcatAxis::DO_NOT_CONCAT
                                            ? genotypeParameters.getNumBitPlanes()
                                            : 1;
        for (auto i = 0; i < num_variants_payloads; ++i) {
            writer.write(variants_payload[i].payloadSize(), 32);
            variants_payload[i].writeCompressed(writer);

            auto variantsPayloadsParams = genotypeParameters.getVariantsPayloadParams();

            if (variantsPayloadsParams[i].sort_variants_rows_flag) {
                writer.write(sort_variants_row_ids_payload[i].payloadSize(), 32);
                sort_variants_row_ids_payload[i].write(writer);
            }
            if (variantsPayloadsParams[i].sort_variants_cols_flag) {
                writer.write(sort_variants_col_ids_payload[i].payloadSize(), 32);
                sort_variants_col_ids_payload[i].write(writer);
            }
            if (genotypeParameters.getBinarizationID() == BinarizationID::ROW_BIN) {
                writer.write(variants_amax_payload.sizeInBytes(), 32);
            }
        }
        auto written = writer.getBitsWritten();
        std::cerr << "genetype payload: " << written << std::endl;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

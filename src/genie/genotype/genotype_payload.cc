/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_payload.h"
#include "genie/entropy/jbig/encoder.h"
#include "genotype_coder.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload(BinMatDtype alleleBinMat) {
    std::vector<genie::genotype::BinMatPayload> variantsPayload;
    codecID = genie::core::AlgoID::JBIG;
    size_t payloadSize = 0;
    uint8_t* payloadArray;
    auto shape = alleleBinMat.shape();
    nrows = (uint32_t)(shape.at(0));
    ncols = static_cast<uint32_t>(shape.at(1));

    genie::genotype::bin_mat_to_bytes(alleleBinMat, &payloadArray, payloadSize);
    payload.resize(payloadSize);
    for (size_t i = 0; i < payloadSize; ++i) {
        payload.at(i) = payloadArray[i];
    }
}

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

    std::stringstream compressedStream;
    if (codecID == core::AlgoID::JBIG) {
        genie::entropy::jbig::JBIGEncoder encoder;
        encoder.encode(payloadStream, compressedStream, ncols, nrows);
        writer.write(&compressedStream);
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
    uint8_t bitsLeftOver = static_cast<uint8_t>(sizeInBytes * 8 - nelements * nbits_per_elem);
    writer.write_reserved(bitsLeftOver);
}

void AmaxPayload::write(core::Writer& writer) const {
    writer.write(nelems, 32);
    writer.write(nbits_per_elem, 8);
    for (uint32_t i = 0; i < nelements; ++i) {
        writer.write(is_one_flag[i], 1);
        if (is_one_flag[i]) writer.write(amax_elements[i], nbits_per_elem);
    }
}

GenotypePayload::GenotypePayload(genie::genotype::EncodingBlock& datablock,
                                 genie::genotype::GenotypeParameters& _genotypeParameters)
    : genotypeParameters(_genotypeParameters),
      phases_payload(datablock.phasing_mat),
      variants_amax_payload(0, 0, 0, {}, {}) {
    for (auto alleleBinMat : datablock.allele_bin_mat_vect) {
        variants_payload.emplace_back(genie::genotype::BinMatPayload(alleleBinMat));
    }

    for (auto i = 0; i < datablock.allele_row_ids_vect.size(); ++i) {
        auto alleleRowIDs = datablock.allele_row_ids_vect.at(i);
        auto shape = datablock.allele_bin_mat_vect.at(i).shape();

        uint32_t nrows = (uint32_t)(shape.at(0));
        uint32_t nBitsPerElem =static_cast<uint32_t>( std::ceil(std::log2(nrows)));

        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleRowIDs) payloadVec.push_back(elem);

        sort_variants_row_ids_payload.emplace_back(
            genie::genotype::RowColIdsPayload(payloadVec.size(), nBitsPerElem, payloadVec));
    }

    for (auto i = 0; i < datablock.allele_col_ids_vect.size(); ++i) {
        auto alleleColIDs = datablock.allele_col_ids_vect.at(i);
        auto shape = datablock.allele_bin_mat_vect.at(i).shape();
        uint32_t ncols = static_cast<uint32_t>(shape.at(1));
        uint32_t nBitsPerElem = static_cast<uint32_t>(std::ceil(std::log2(ncols)));

        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleColIDs) payloadVec.push_back(elem);
        sort_variants_col_ids_payload.emplace_back(
            genie::genotype::RowColIdsPayload(payloadVec.size(), nBitsPerElem, payloadVec));
    }
}

void genie::genotype::GenotypePayload::write(core::Writer& writer) const {
    uint8_t num_variants_payloads = genotypeParameters.getBinarizationID() == BinarizationID::BIT_PLANE &&
                                            genotypeParameters.isConcatenated() == ConcatAxis::DO_NOT_CONCAT
                                        ? genotypeParameters.getNumBitPlanes()
                                        : 1;
    auto indecRowIds = 0;
    auto indecColIds = 0;
    for (auto i = 0; i < num_variants_payloads; ++i) {
        std::stringstream tempstream;
        core::Writer writesize(&tempstream);
        variants_payload[i].writeCompressed(writesize);
        auto variantssize = tempstream.str().size();
        writer.write(variantssize, 32);
        variants_payload[i].writeCompressed(writer);
        auto variantsPayloadsParams = genotypeParameters.getVariantsPayloadParams();
        if (variantsPayloadsParams[i].sort_variants_rows_flag) {
            uint32_t size = sort_variants_row_ids_payload[indecRowIds].getWriteSizeInBytes();
            writer.write(size, 32);
            sort_variants_row_ids_payload[indecRowIds].write(writer);
            ++indecRowIds;
        }
        if (variantsPayloadsParams[i].sort_variants_cols_flag) {
            auto size = sort_variants_col_ids_payload[indecColIds].getWriteSizeInBytes();
            writer.write(size, 32);
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
}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
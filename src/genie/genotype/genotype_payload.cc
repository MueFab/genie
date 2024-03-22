/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_payload.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

#include "genotype_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

BinMatPayload::BinMatPayload(BinMatDtype binMat) : BinMatPayload(binMat, genie::core::AlgoID::JBIG) {}

BinMatPayload::BinMatPayload(BinMatDtype binMat, core::AlgoID _codecID) : codecID(_codecID) {
    auto shape = binMat.shape();
    nrows = (uint32_t)(shape.at(0));
    ncols = static_cast<uint32_t>(shape.at(1));

    std::vector<bool> phases;
    for (auto phasing : binMat) {
        phases.push_back(phasing);
    }

    size_t payloadSize = 0;
    uint8_t* payloadArray;
    genie::genotype::bin_mat_to_bytes(binMat, &payloadArray, payloadSize);
    payload.resize(payloadSize);
    for (size_t i = 0; i < payloadSize; ++i) {
        payload.at(i) = payloadArray[i];
    }
}

void BinMatPayload::write(core::Writer& writer) const {
    writer.write(nrows, 32);
    writer.write(ncols, 32);
    for (auto byte : payload) writer.write(byte, 8);
}

void BinMatPayload::writeCompressed(core::Writer& writer) const {
    std::stringstream payloadStream;
    for (auto byte : payload) payloadStream.write((char*)&byte, 1);

    std::stringstream compressedStream;
    if (codecID != genie::core::AlgoID::JBIG) {
        writer.write(nrows, 32);
        writer.write(ncols, 32);
    }

    switch (codecID) {
        case genie::core::AlgoID::LZMA: {
            genie::entropy::lzma::LZMAEncoder encoder;
            encoder.encode(payloadStream, compressedStream);
            writer.write(&compressedStream);
        } break;
        case genie::core::AlgoID::ZSTD: {
            genie::entropy::zstd::ZSTDEncoder encoder;
            encoder.encode(payloadStream, compressedStream);
            writer.write(&compressedStream);
        } break;
        case genie::core::AlgoID::BSC: {
            genie::entropy::bsc::BSCEncoder encoder;
            encoder.encode(payloadStream, compressedStream);
            writer.write(&compressedStream);
        } break;
        case genie::core::AlgoID::JBIG: {
            genie::entropy::jbig::JBIGEncoder encoder;
            encoder.encode(payloadStream, compressedStream, ncols, nrows);
            writer.write(&compressedStream);
        } break;
        default:
            writer.write(&payloadStream);
            break;
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
    uint64_t sizeInBits = 32 + 4;
    writer.write(nelems, 32);
    writer.write(nbits_per_elem, 4);
    for (uint32_t i = 0; i < nelements; ++i) {
        sizeInBits++;
        if (amax_elements[i] == 0) {
            writer.write(1, 1);
        } else {
            writer.write(0, 1);
            writer.write(amax_elements[i] - 1, nbits_per_elem);
            sizeInBits += nbits_per_elem;
        }
    }
    uint8_t remain = sizeInBits % 8;
    if (remain != 0) writer.write_reserved(8 - remain);
}

GenotypePayload::GenotypePayload(genie::genotype::EncodingBlock& datablock,
                                 genie::genotype::GenotypeParameters& _genotypeParameters)
    : genotypeParameters(_genotypeParameters),
      phases_payload(datablock.phasing_mat, _genotypeParameters.getPhasesPayloadParams().variants_codec_ID),
      variants_amax_payload(0, 0, 0, {}) {
    size_t index = 0;
    for (auto alleleBinMat : datablock.allele_bin_mat_vect) {
        variants_payload.emplace_back(genie::genotype::BinMatPayload(
            alleleBinMat, genotypeParameters.getVariantsPayloadParams().at(index++).variants_codec_ID));
    }

    for (size_t i = 0; i < datablock.allele_row_ids_vect.size(); ++i) {
        auto alleleRowIDs = datablock.allele_row_ids_vect.at(i);
        auto shape = datablock.allele_bin_mat_vect.at(i).shape();

        uint32_t nrows = (uint32_t)(shape.at(0));
        uint32_t nBitsPerElem = static_cast<uint32_t>(std::ceil(std::log2(nrows)));

        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleRowIDs) payloadVec.push_back(elem);

        sort_variants_row_ids_payload.emplace_back(
            genie::genotype::RowColIdsPayload(payloadVec.size(), nBitsPerElem, payloadVec));
    }

    for (size_t i = 0; i < datablock.allele_col_ids_vect.size(); ++i) {
        auto alleleColIDs = datablock.allele_col_ids_vect.at(i);
        auto shape = datablock.allele_bin_mat_vect.at(i).shape();
        uint32_t ncols = static_cast<uint32_t>(shape.at(1));
        uint32_t nBitsPerElem = static_cast<uint32_t>(std::ceil(std::log2(ncols)));

        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleColIDs) payloadVec.push_back(elem);
        sort_variants_col_ids_payload.emplace_back(
            genie::genotype::RowColIdsPayload(payloadVec.size(), nBitsPerElem, payloadVec));
    }
    if (genotypeParameters.getBinarizationID() == genie::genotype::BinarizationID::ROW_BIN) {
        std::vector<uint64_t> amax_elements;
        for (auto amax : datablock.amax_vec) {
            amax_elements.push_back(amax);
        }
        uint32_t elems = static_cast<uint32_t>(amax_elements.size());
        uint8_t nbits_per_elem = 8;
        AmaxPayload amax_payload(elems, nbits_per_elem, elems, amax_elements);
        variants_amax_payload = amax_payload;
    }
}

void genie::genotype::GenotypePayload::write(core::Writer& writer) const {
    size_t indecRowIds = 0;
    size_t indecColIds = 0;
    uint8_t num_variants_payloads = genotypeParameters.getBinarizationID() == BinarizationID::BIT_PLANE &&
                                            genotypeParameters.getConcatAxis() == ConcatAxis::DO_NOT_CONCAT
                                        ? genotypeParameters.getNumBitPlanes()
                                        : 1;
    for (auto i = 0; i < num_variants_payloads; ++i) {
        std::stringstream tempstream;
        core::Writer writesize(&tempstream);
        variants_payload[i].writeCompressed(writesize);
        auto variantssize = tempstream.str().size();
        writer.write(variantssize, 32);
        variants_payload[i].writeCompressed(writer);
        auto variantsPayloadsParams = genotypeParameters.getVariantsPayloadParams();
        if (variantsPayloadsParams[i].sort_rows_flag) {
            uint32_t size = sort_variants_row_ids_payload[indecRowIds].getWriteSizeInBytes();
            writer.write(size, 32);
            sort_variants_row_ids_payload[indecRowIds].write(writer);
            ++indecRowIds;
        }
        if (variantsPayloadsParams[i].sort_cols_flag) {
            auto size = sort_variants_col_ids_payload[indecColIds].getWriteSizeInBytes();
            writer.write(size, 32);
            sort_variants_col_ids_payload[indecColIds].write(writer);
            ++indecColIds;
        }
        if (genotypeParameters.getBinarizationID() == BinarizationID::ROW_BIN) {
            writer.write(variants_amax_payload.sizeInBytes(), 32);
            variants_amax_payload.write(writer);
        }
    }
    if (genotypeParameters.isPhaseEncoded()) {
        std::stringstream tempstream;
        core::Writer writesize(&tempstream);
        phases_payload.writeCompressed(writesize);
        auto phasesSize = tempstream.str().size();
        writer.write(phasesSize, 32);
        phases_payload.writeCompressed(writer);
    }
}
}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
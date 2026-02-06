/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_coder_xtensor.h"
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/likelihood/likelihood_coder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::likelihood::detail::xtensor {

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, EncodingBlock& block,
                         std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().GetSampleCount();
    uint8_t num_likelihoods = recs.front().GetNumberOfLikelihoods();

    block.likelihood_mat.assign(block_size, std::vector<uint32_t>(num_samples * num_likelihoods));

    for (uint32_t i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.GetSampleCount(), "Number of samples is not constant within a block!");
        UTILS_DIE_IF(num_likelihoods != rec.GetNumberOfLikelihoods(),
                     "Number of likelihoods is not constant within a block!");

        auto& rec_likelihoods = rec.GetLikelihoods();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            for (uint8_t k_likelihood = 0; k_likelihood < num_likelihoods; k_likelihood++) {
                block.likelihood_mat[i_rec][j_sample * num_likelihoods + k_likelihood] =
                    rec_likelihoods[j_sample][k_likelihood];
            }
        }
    }

    block.nrows = block_size;
    block.ncols = num_samples * num_likelihoods;
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    if (opt.transform_flag) {
        detail::xtensor::transform_lut(block.likelihood_mat, block.lut, block.nelems, block.idx_mat, block.dtype_id);
    } else {
        block.idx_mat = block.likelihood_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    if (opt.transform_flag) {
        detail::xtensor::inverse_transform_lut(block.likelihood_mat, block.lut, block.idx_mat);
    } else {
        block.likelihood_mat = block.idx_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id) {
    auto m = likelihood_mat.size();
    auto n = (m == 0) ? 0 : likelihood_mat[0].size();

    // Flatten and convert to xtensor
    std::vector<uint32_t> flat_mat;
    flat_mat.reserve(m * n);
    for (const auto& row : likelihood_mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
    
    auto xt_likelihood_mat = xt::adapt(flat_mat, std::vector<size_t>{m, n});
    auto xt_lut = xt::unique(xt_likelihood_mat);
    auto xt_idx_mat = xt::xtensor<uint32_t, 2>({m, n});

    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            auto likelihood_val = likelihood_mat[i][j];

            // Binary Search Algorithm
            uint32_t low = 0;
            uint32_t high = static_cast<uint32_t>(xt_lut.shape(0)) - 1;

            while (low <= high) {
                uint32_t idx = (low + high) / 2;
                if (xt_lut[idx] > likelihood_val)
                    high = idx - 1;
                else if (xt_lut[idx] < likelihood_val)
                    low = idx + 1;
                else {
                    xt_idx_mat(i, j) = idx;
                    break;
                }
            }
        }
    }

    // Convert back to std::vector
    lut.assign(xt_lut.begin(), xt_lut.end());
    idx_mat.assign(m, std::vector<uint32_t>(n));
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            idx_mat[i][j] = xt_idx_mat(i, j);
        }
    }

    nelems = (uint32_t)lut.size();
    if (nelems < (1 << 8)) {
        dtype_id = core::DataType::UINT8;
    } else if (nelems < (1 << 16)) {
        dtype_id = core::DataType::UINT16;
    } else {
        dtype_id = core::DataType::UINT32;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    auto m = idx_mat.size();
    auto n = (m == 0) ? 0 : idx_mat[0].size();

    likelihood_mat.assign(m, std::vector<uint32_t>(n));

    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            likelihood_mat[i][j] = lut[idx_mat[i][j]];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_mat(UInt32MatDtype mat, const core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload) {
    nrows = (uint32_t)mat.size();
    ncols = (uint32_t)(mat.empty() ? 0 : mat[0].size());

    util::BitWriter writer(&payload);

    if (dtype_id == core::DataType::UINT8) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint8_t>(static_cast<uint8_t>(mat[i][j]));
            }
        }
    } else if (dtype_id == core::DataType::UINT16) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint16_t>(static_cast<uint16_t>(mat[i][j]));
            }
        }
    } else if (dtype_id == core::DataType::UINT32) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint32_t>(static_cast<uint32_t>(mat[i][j]));
            }
        }
    } else
        UTILS_DIE("Invalid DataType");
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_arr(UInt32ArrDtype arr, const uint32_t nelems, std::stringstream& payload) {
    util::BitWriter writer(&payload);

    for (size_t i = 0; i < nelems; i++) {
        writer.WriteBypassBE<uint32_t>(static_cast<uint32_t>(arr[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_likelihood(std::vector<core::record::VariantGenotype>& recs,

                       LikelihoodParameters& params, LikelihoodPayload& payload,

                       size_t block_size, bool transform_flag) {

    EncodingOptions opt = {(uint32_t)block_size, transform_flag};

    EncodingBlock block;

    detail::xtensor::extract_likelihoods(opt, block, recs);

    detail::xtensor::transform_likelihood_mat(opt, block);

    detail::xtensor::serialize_mat(block.idx_mat, block.dtype_id, block.nrows, block.ncols, block.serialized_mat);

    detail::xtensor::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    block.serialized_mat.seekp(0, std::ios::end);
    if (recs.at(0).GetNumberOfLikelihoods() > 0) {
        genie::entropy::lzma::LZMAEncoder lzmaEncoder;
        std::stringstream compressedData;
        lzmaEncoder.encode(block.serialized_arr, compressedData);

        block.serialized_arr.str("");
        block.serialized_arr << compressedData.rdbuf();
        compressedData.str("");

        lzmaEncoder.encode(block.serialized_mat, compressedData);
        block.serialized_mat.str("");
        block.serialized_mat << compressedData.rdbuf();
    }

    params = LikelihoodParameters(static_cast<uint8_t>(recs.at(0).GetNumberOfLikelihoods()), transform_flag, block.dtype_id);

    payload.setNRows(block.nrows);
    payload.setNCols(block.ncols);
    const std::string& str = block.serialized_mat.str();
    const std::vector<uint8_t> vec(str.begin(), str.end());
    payload.setPayload(vec);
}

// ---------------------------------------------------------------------------------------------------------------------

void deserialize_mat(std::vector<uint8_t> payload, const core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                     UInt32MatDtype& mat) {
    std::string s(payload.begin(), payload.end());
    std::stringstream stream(s);
    util::BitReader reader(stream);

    mat.assign(nrows, std::vector<uint32_t>(ncols));

    if (dtype_id == core::DataType::UINT8) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat[i][j] = reader.Read<uint8_t>();
            }
        }
    } else if (dtype_id == core::DataType::UINT16) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat[i][j] = reader.Read<uint16_t>();
            }
        }
    } else if (dtype_id == core::DataType::UINT32) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat[i][j] = reader.Read<uint32_t>();
            }
        }
    } else
        UTILS_DIE("Invalid DataType");
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_likelihood(const LikelihoodParameters& params, LikelihoodPayload& payload,
                       std::vector<core::record::VariantGenotype>& recs) {
    EncodingOptions opt = {(uint32_t)payload.getNRows(), params.GetTransformFlag()};
    EncodingBlock block;
    block.nrows = payload.getNRows();
    block.ncols = payload.getNCols();
    block.dtype_id = params.GetDtypeId();
    
    detail::xtensor::deserialize_mat(payload.getPayload(), block.dtype_id, block.nrows, block.ncols, block.idx_mat);
    detail::xtensor::inverse_transform_likelihood_mat(opt, block);
    for (uint32_t i = 0; i < block.nrows; ++i) {
        recs.emplace_back();
        std::vector<std::vector<uint32_t>> likelihoods;
        for (uint32_t j = 0; j < block.ncols; j += params.GetNumGlPerSample()) {
            std::vector<uint32_t> sample_likelihoods;
            for (uint8_t k = 0; k < params.GetNumGlPerSample(); ++k) {
                sample_likelihoods.push_back(block.likelihood_mat[i][j + k]);
            }
            likelihoods.push_back(sample_likelihoods);
        }
        recs.back().SetLikelihoods(std::move(likelihoods));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::likelihood::detail::xtensor

// ---------------------------------------------------------------------------------------------------------------------

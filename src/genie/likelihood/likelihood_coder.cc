/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_coder.h"
#include <xtensor/xsort.hpp>
#include "genie/entropy/lzma/encoder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace likelihood {

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, EncodingBlock& block,
                         std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().GetSampleCount();
    uint8_t num_likelihoods = recs.front().GetNumberOfLikelihoods();

    auto& likelihood_mat = block.likelihood_mat;

    MatShapeDtype likelihood_mat_shape = {block_size, num_samples * num_likelihoods};
    likelihood_mat = xt::empty<uint32_t, xt::layout_type::row_major>(likelihood_mat_shape);

    for (uint32_t i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];

        UTILS_DIE_IF(num_samples != rec.GetSampleCount(), "Number of samples is not constant within a block!");
        UTILS_DIE_IF(num_likelihoods != rec.GetNumberOfLikelihoods(),
                     "Number of likelihoods is not constant within a block!");

        auto& rec_likelihoods = rec.GetLikelihoods();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            for (uint8_t k_likelihood = 0; k_likelihood < num_likelihoods; k_likelihood++) {
                likelihood_mat(i_rec, j_sample * num_likelihoods + k_likelihood) =
                    rec_likelihoods[j_sample][k_likelihood];
            }
        }
    }

    block.nrows = (uint32_t)likelihood_mat.shape(0);
    block.ncols = (uint32_t)likelihood_mat.shape(1);
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    if (opt.transform_flag) {
        transform_lut(block.likelihood_mat, block.lut, block.nelems, block.idx_mat, block.dtype_id);
    } else {
        block.idx_mat = xt::empty_like(block.likelihood_mat);
        xt::view(block.idx_mat, xt::all(), xt::all()) = xt::view(block.likelihood_mat, xt::all(), xt::all());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    if (opt.transform_flag) {
        inverse_transform_lut(block.likelihood_mat, block.lut, block.idx_mat);
    } else {
        block.likelihood_mat = xt::empty_like(block.idx_mat);
        auto ndim = block.likelihood_mat.dimension();
        auto dim0 = block.likelihood_mat.shape(0);
        auto dim1 = block.likelihood_mat.shape(1);
        (void)ndim;
        (void)dim0;
        (void)dim1;
        xt::view(block.likelihood_mat, xt::all(), xt::all()) = xt::view(block.idx_mat, xt::all(), xt::all());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id) {
    auto m = likelihood_mat.shape(0);
    auto n = likelihood_mat.shape(1);

    idx_mat = xt::xtensor<uint32_t, 2>({m, n});
    lut = xt::unique(likelihood_mat);

    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            auto& likelihood_val = likelihood_mat(i, j);

            // Binary Search Algorithm
            uint32_t low = 0;
            uint32_t high = static_cast<uint32_t>(lut.shape(0)) - 1;

            while (low <= high) {
                uint32_t idx = (low + high) / 2;
                if (lut[idx] > likelihood_val)
                    high = idx - 1;
                else if (lut[idx] < likelihood_val)
                    low = idx + 1;
                else {
                    idx_mat(i, j) = idx;
                    break;
                }
            }
        }
    }

    nelems = (uint32_t)lut.size();
    if (nelems < (1 << 8)) {
        dtype_id = core::DataType::UINT8;
    } else if (nelems < (1 << 16)) {
        dtype_id = core::DataType::UINT16;
    } else if (nelems < ((size_t)1 << 32)) {
        dtype_id = core::DataType::UINT32;
    } else {
        UTILS_DIE("Invalid DataType");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    auto num_dim = lut.dimension();

    UTILS_DIE_IF(num_dim != 1, "LUT dimension must be 1!");
    auto num_unique_vals = lut.shape(0);

    likelihood_mat = xt::empty_like(idx_mat);
    auto m = likelihood_mat.shape(0);
    auto n = likelihood_mat.shape(1);

    if (num_unique_vals > 1) {
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < n; j++) {
                likelihood_mat(i, j) = lut(idx_mat(i, j));
            }
        }
    } else {
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < n; j++) {
                likelihood_mat(i, j) = lut(i, j);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_mat(UInt32MatDtype mat, const core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload) {
    nrows = (uint32_t)mat.shape(0);
    ncols = (uint32_t)mat.shape(1);

    util::BitWriter writer(&payload);

    if (dtype_id == core::DataType::UINT8) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint8_t>(static_cast<uint8_t>(mat(i, j)));
            }
        }
    } else if (dtype_id == core::DataType::UINT16) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint16_t>(static_cast<uint16_t>(mat(i, j)));
            }
        }
    } else if (dtype_id == core::DataType::UINT32) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                writer.WriteBypassBE<uint32_t>(static_cast<uint32_t>(mat(i, j)));
            }
        }
    } else
        UTILS_DIE("Invalid DataType");
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_arr(UInt32ArrDtype arr, const uint32_t nelems, std::stringstream& payload) {
    util::BitWriter writer(&payload);

    for (size_t i = 0; i < nelems; i++) {
        writer.WriteBypassBE<uint32_t>(static_cast<uint32_t>(arr(i)));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_likelihood(std::vector<core::record::VariantGenotype>& recs, LikelihoodParameters& params,
                       LikelihoodPayload& payload, size_t block_size, bool transform_flag) {
    EncodingOptions opt = {(uint32_t)block_size, transform_flag};
    EncodingBlock block;
    extract_likelihoods(opt, block, recs);
    transform_likelihood_mat(opt, block);
    genie::likelihood::serialize_mat(block.idx_mat, block.dtype_id, block.nrows, block.ncols, block.serialized_mat);
    genie::likelihood::serialize_arr(block.lut, block.nelems, block.serialized_arr);
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

    if (dtype_id == core::DataType::UINT8) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat(i, j) = reader.Read<uint8_t>();
            }
        }
    } else if (dtype_id == core::DataType::UINT16) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat(i, j) = reader.Read<uint16_t>();
            }
        }
    } else if (dtype_id == core::DataType::UINT32) {
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                mat(i, j) = reader.Read<uint32_t>();
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
    MatShapeDtype likelihood_mat_shape = {block.nrows, block.ncols};
    block.idx_mat = xt::empty<uint32_t, xt::layout_type::row_major>(likelihood_mat_shape);
    deserialize_mat(payload.getPayload(), block.dtype_id, block.nrows, block.ncols, block.idx_mat);
    inverse_transform_likelihood_mat(opt, block);
    for (uint32_t i = 0; i < block.nrows; ++i) {
        recs.emplace_back();
        std::vector<std::vector<uint32_t>> likelihoods;
        for (uint32_t j = 0; j < block.ncols; j += params.GetNumGlPerSample()) {
            std::vector<uint32_t> sample_likelihoods;
            for (uint8_t k = 0; k < params.GetNumGlPerSample(); ++k) {
                sample_likelihoods.push_back(block.likelihood_mat(i, j + k));
            }
            likelihoods.push_back(sample_likelihoods);
        }
        recs.back().SetLikelihoods(std::move(likelihoods));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace likelihood
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

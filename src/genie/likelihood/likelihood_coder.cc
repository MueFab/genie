/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/likelihood/likelihood_coder.h"
#include <algorithm>
#include <cmath>
#include <set>

#ifdef GENIE_BACKEND_XTENSOR
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xrandom.hpp>
#elif defined(GENIE_BACKEND_EIGEN)
#include <Eigen/Core>
#endif

#include "genie/entropy/lzma/encoder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_reader.h"
#include "genie/backend/backend.h"

namespace genie::likelihood {

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods(const EncodingOptions& opt, LikelihoodEncodingBlock& block,
                         std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().GetSampleCount();
    uint8_t num_likelihoods = recs.front().GetNumberOfLikelihoods();
    uint32_t ncols = num_samples * num_likelihoods;

    ::genie::backend::resize_mat(block.likelihood_mat, std::vector<size_t>{static_cast<size_t>(block_size), static_cast<size_t>(ncols)});
    for(size_t i=0; i<block_size; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(block.likelihood_mat, i, j, 0u);

    for (uint32_t i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];
        UTILS_DIE_IF(num_samples != rec.GetSampleCount(), "Number of samples is not constant within a block!");
        UTILS_DIE_IF(num_likelihoods != rec.GetNumberOfLikelihoods(),
                     "Number of likelihoods is not constant within a block!");

        auto& rec_likelihoods = rec.GetLikelihoods();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            for (uint8_t k_likelihood = 0; k_likelihood < num_likelihoods; k_likelihood++) {
#ifdef GENIE_BACKEND_XTENSOR
                block.likelihood_mat(i_rec, j_sample * num_likelihoods + k_likelihood) =
                    rec_likelihoods[j_sample][k_likelihood];
#elif defined(GENIE_BACKEND_EIGEN)
                block.likelihood_mat(static_cast<Eigen::Index>(i_rec), static_cast<Eigen::Index>(j_sample * num_likelihoods + k_likelihood)) =
                    rec_likelihoods[j_sample][k_likelihood];
#else
                block.likelihood_mat[i_rec][j_sample * num_likelihoods + k_likelihood] =
                    rec_likelihoods[j_sample][k_likelihood];
#endif
            }
        }
    }

    block.nrows = static_cast<uint32_t>(block_size);
    block.ncols = ncols;
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    if (!opt.transform_flag || block.nrows == 0) return;

#ifdef GENIE_BACKEND_XTENSOR
    std::set<uint32_t> unique_values;
    for (auto val : block.likelihood_mat) {
        unique_values.insert(val);
    }

    block.nelems = static_cast<uint32_t>(unique_values.size());
    block.lut = xt::adapt(std::vector<uint32_t>(unique_values.begin(), unique_values.end()));
    block.idx_mat = xt::zeros<uint32_t>({block.nrows, block.ncols});

    for (uint32_t i = 0; i < block.nrows; i++) {
        for (uint32_t j = 0; j < block.ncols; j++) {
            auto it = unique_values.find(block.likelihood_mat(i, j));
            block.idx_mat(i, j) = static_cast<uint32_t>(std::distance(unique_values.begin(), it));
        }
    }
#elif defined(GENIE_BACKEND_EIGEN)
    // Eigen implementation
    std::set<uint32_t> unique_values;
    for (int i = 0; i < block.likelihood_mat.rows(); ++i) {
        for (int j = 0; j < block.likelihood_mat.cols(); ++j) {
            unique_values.insert(block.likelihood_mat(i, j));
        }
    }

    block.nelems = static_cast<uint32_t>(unique_values.size());
    block.lut.resize(static_cast<Eigen::Index>(block.nelems));
    size_t k = 0;
    for (auto v : unique_values) block.lut(static_cast<Eigen::Index>(k++)) = v;

    block.idx_mat.resize(block.likelihood_mat.rows(), block.likelihood_mat.cols());
    for (int i = 0; i < block.likelihood_mat.rows(); ++i) {
        for (int j = 0; j < block.likelihood_mat.cols(); ++j) {
            auto it = unique_values.find(block.likelihood_mat(i, j));
            block.idx_mat(i, j) = static_cast<uint32_t>(std::distance(unique_values.begin(), it));
        }
    }
#else
    std::set<uint32_t> unique_values;
    for (const auto& row : block.likelihood_mat) {
        for (auto val : row) {
            unique_values.insert(val);
        }
    }

    block.nelems = static_cast<uint32_t>(unique_values.size());
    block.lut.assign(unique_values.begin(), unique_values.end());
    block.idx_mat.assign(block.nrows, std::vector<uint32_t>(block.ncols));

    for (uint32_t i = 0; i < block.nrows; i++) {
        for (uint32_t j = 0; j < block.ncols; j++) {
            auto it = unique_values.find(block.likelihood_mat[i][j]);
            block.idx_mat[i][j] = static_cast<uint32_t>(std::distance(unique_values.begin(), it));
        }
    }
#endif

    if (block.nelems <= 256) {
        block.dtype_id = core::DataType::UINT8;
    } else if (block.nelems <= 65536) {
        block.dtype_id = core::DataType::UINT16;
    } else {
        block.dtype_id = core::DataType::UINT32;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    if (!opt.transform_flag || block.nrows == 0) return;

#ifdef GENIE_BACKEND_XTENSOR
    block.likelihood_mat = xt::zeros<uint32_t>({block.nrows, block.ncols});
    for (uint32_t i = 0; i < block.nrows; i++) {
        for (uint32_t j = 0; j < block.ncols; j++) {
            block.likelihood_mat(i, j) = block.lut(block.idx_mat(i, j));
        }
    }
#elif defined(GENIE_BACKEND_EIGEN)
    block.likelihood_mat.resize(block.nrows, block.ncols);
    for (uint32_t i = 0; i < block.nrows; i++) {
        for (uint32_t j = 0; j < block.ncols; j++) {
            block.likelihood_mat(i, j) = block.lut(block.idx_mat(i, j));
        }
    }
#else
    block.likelihood_mat.assign(block.nrows, std::vector<uint32_t>(block.ncols));
    for (uint32_t i = 0; i < block.nrows; i++) {
        for (uint32_t j = 0; j < block.ncols; j++) {
            block.likelihood_mat[i][j] = block.lut[block.idx_mat[i][j]];
        }
    }
#endif
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_block(LikelihoodEncodingBlock& block, bool transform_flag) {
    if (transform_flag) {
        ::genie::backend::serialize_mat(block.idx_mat, static_cast<uint32_t>(block.dtype_id), block.nrows, block.ncols,
                                       block.serialized_mat);
        ::genie::backend::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    } else {
        ::genie::backend::serialize_mat(block.likelihood_mat, static_cast<uint32_t>(block.dtype_id), block.nrows,
                                       block.ncols, block.serialized_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void deserialize_block(const std::vector<uint8_t>& mat_payload, const std::vector<uint8_t>& lut_payload,
                       LikelihoodEncodingBlock& block, bool transform_flag) {
    if (transform_flag) {
        ::genie::backend::deserialize_mat(mat_payload, static_cast<uint32_t>(block.dtype_id), block.nrows, block.ncols,
                                         block.idx_mat);
        ::genie::backend::deserialize_arr(lut_payload, block.nelems, block.lut);
    } else {
        ::genie::backend::deserialize_mat(mat_payload, static_cast<uint32_t>(block.dtype_id), block.nrows, block.ncols,
                                         block.likelihood_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_likelihood(std::vector<core::record::VariantGenotype>& recs, LikelihoodParameters& params,
                       LikelihoodPayload& payload, size_t block_size, bool transform_flag) {
    EncodingOptions opt{static_cast<uint32_t>(block_size), transform_flag};
    LikelihoodEncodingBlock block;

    extract_likelihoods(opt, block, recs);
    transform_likelihood_mat(opt, block);

    serialize_block(block, transform_flag);

    payload.setNRows(block.nrows);
    payload.setNCols(block.ncols);
    payload.setTransformFlag(transform_flag);

    std::vector<uint8_t> mat_data;
    std::string s_mat = block.serialized_mat.str();
    mat_data.assign(s_mat.begin(), s_mat.end());
    payload.setPayload(std::move(mat_data));

    if (transform_flag) {
        std::vector<uint8_t> lut_data;
        std::string s_lut = block.serialized_arr.str();
        lut_data.assign(s_lut.begin(), s_lut.end());
        payload.setAdditionalPayload(std::move(lut_data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_likelihood(const LikelihoodParameters& params, LikelihoodPayload& payload,
                       std::vector<core::record::VariantGenotype>& recs) {
    LikelihoodEncodingBlock block;
    block.nrows = payload.getNRows();
    block.ncols = payload.getNCols();

    bool transform_flag = payload.getTransformFlag();
    if (transform_flag) {
        block.nelems = static_cast<uint32_t>(payload.getAdditionalPayload().size() / 4);
        if (block.nelems <= 256) {
            block.dtype_id = core::DataType::UINT8;
        } else if (block.nelems <= 65536) {
            block.dtype_id = core::DataType::UINT16;
        } else {
            block.dtype_id = core::DataType::UINT32;
        }
    } else {
        block.dtype_id = core::DataType::UINT32;
    }

    EncodingOptions opt{block.nrows, transform_flag};

    deserialize_block(payload.getPayload(), payload.getAdditionalPayload(), block, transform_flag);
    inverse_transform_likelihood_mat(opt, block);

    uint32_t num_samples = recs.front().GetSampleCount();
    uint8_t num_likelihoods = recs.front().GetNumberOfLikelihoods();

    for (uint32_t i = 0; i < block.nrows; i++) {
        std::vector<std::vector<uint32_t>> rec_likelihoods(num_samples, std::vector<uint32_t>(num_likelihoods));
        for (uint32_t j = 0; j < num_samples; j++) {
            for (uint8_t k = 0; k < num_likelihoods; k++) {
                rec_likelihoods[j][k] = ::genie::backend::get_mat_element(block.likelihood_mat, i, j * num_likelihoods + k);
            }
        }
        recs[i].SetLikelihoods(rec_likelihoods);
    }
}

} // namespace genie::likelihood

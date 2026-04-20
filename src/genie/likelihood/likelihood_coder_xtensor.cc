/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
#include "genie/likelihood/likelihood_coder_xtensor.h"
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xrandom.hpp>
#include "genie/likelihood/likelihood_coder.h"
=======
#include "likelihood_coder.h"
#include <genie/backend/backend.h>
#include <algorithm>
#include <set>
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
#include "genie/entropy/lzma/encoder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_reader.h"

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
// ---------------------------------------------------------------------------------------------------------------------

namespace genie::likelihood::detail::xtensor {
=======
namespace genie::likelihood::detail {
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc

// ---------------------------------------------------------------------------------------------------------------------

void extract_likelihoods_impl(const EncodingOptions& opt, LikelihoodEncodingBlock& block,
                               std::vector<core::record::VariantGenotype>& recs) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto block_size = opt.block_size < recs.size() ? opt.block_size : recs.size();
    uint32_t num_samples = recs.front().GetSampleCount();
    uint8_t num_likelihoods = recs.front().GetNumberOfLikelihoods();
    uint32_t ncols = num_samples * num_likelihoods;

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
    block.likelihood_mat.assign(block_size, std::vector<uint32_t>(num_samples * num_likelihoods));
=======
    ::genie::backend::resize_mat(block.likelihood_mat, std::vector<size_t>{block_size, ncols});
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc

    for (uint32_t i_rec = 0; i_rec < block_size; i_rec++) {
        auto& rec = recs[i_rec];
        UTILS_DIE_IF(num_samples != rec.GetSampleCount(), "Number of samples is not constant within a block!");
        UTILS_DIE_IF(num_likelihoods != rec.GetNumberOfLikelihoods(),
                     "Number of likelihoods is not constant within a block!");

        auto& rec_likelihoods = rec.GetLikelihoods();
        for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
            for (uint8_t k_likelihood = 0; k_likelihood < num_likelihoods; k_likelihood++) {
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
                block.likelihood_mat[i_rec][j_sample * num_likelihoods + k_likelihood] =
                    rec_likelihoods[j_sample][k_likelihood];
=======
                ::genie::backend::set_mat_element(block.likelihood_mat, i_rec, j_sample * num_likelihoods + k_likelihood,
                    rec_likelihoods[j_sample][k_likelihood]);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
            }
        }
    }

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
    block.nrows = block_size;
    block.ncols = num_samples * num_likelihoods;
=======
    block.nrows = static_cast<uint32_t>(block_size);
    block.ncols = ncols;
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_likelihood_mat_impl(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    if (opt.transform_flag) {
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
        detail::xtensor::transform_lut(block.likelihood_mat, block.lut, block.nelems, block.idx_mat, block.dtype_id);
=======
        transform_lut_impl(block.likelihood_mat, block.lut, block.nelems, block.idx_mat, block.dtype_id);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
    } else {
        block.idx_mat = block.likelihood_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_likelihood_mat_impl(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    if (opt.transform_flag) {
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
        detail::xtensor::inverse_transform_lut(block.likelihood_mat, block.lut, block.idx_mat);
=======
        inverse_transform_lut_impl(block.likelihood_mat, block.lut, block.idx_mat);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
    } else {
        block.likelihood_mat = block.idx_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_lut_impl(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id) {
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
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
=======
    size_t dim_m = ::genie::backend::get_mat_shape(likelihood_mat, 0);
    size_t n = ::genie::backend::get_mat_shape(likelihood_mat, 1);

    std::set<uint32_t> unique_vals;
    for (size_t idx_i = 0; idx_i < dim_m; idx_i++) {
        for (size_t idx_j = 0; idx_j < n; idx_j++) {
            unique_vals.insert(::genie::backend::get_mat_element(likelihood_mat, idx_i, idx_j));
        }
    }

    std::vector<uint32_t> lut_vec(unique_vals.begin(), unique_vals.end());
    ::genie::backend::resize_arr(lut, lut_vec.size());
    for (size_t idx_i = 0; idx_i < lut_vec.size(); idx_i++) {
        ::genie::backend::set_arr_element(lut, idx_i, lut_vec[idx_i]);
    }

    nelems = (uint32_t)lut_vec.size();

    ::genie::backend::resize_mat(idx_mat, std::vector<size_t>{dim_m, n});
    for (size_t idx_i = 0; idx_i < dim_m; idx_i++) {
        for (size_t idx_j = 0; idx_j < n; idx_j++) {
            uint32_t val = ::genie::backend::get_mat_element(likelihood_mat, idx_i, idx_j);
            auto it = std::lower_bound(lut_vec.begin(), lut_vec.end(), val);
            ::genie::backend::set_mat_element(idx_mat, idx_i, idx_j, (uint32_t)std::distance(lut_vec.begin(), it));
        }
    }

    if (nelems < (1u << 8)) {
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
        dtype_id = core::DataType::UINT8;
    } else if (nelems < (1u << 16)) {
        dtype_id = core::DataType::UINT16;
    } else {
        dtype_id = core::DataType::UINT32;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    auto m = idx_mat.size();
    auto n = (m == 0) ? 0 : idx_mat[0].size();

    likelihood_mat.assign(m, std::vector<uint32_t>(n));

    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            likelihood_mat[i][j] = lut[idx_mat[i][j]];
=======
void inverse_transform_lut_impl(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    size_t dim_m = ::genie::backend::get_mat_shape(idx_mat, 0);
    size_t n = ::genie::backend::get_mat_shape(idx_mat, 1);
    size_t unique_count = ::genie::backend::get_arr_size(lut);

    ::genie::backend::resize_mat(likelihood_mat, std::vector<size_t>{dim_m, n});

    if (unique_count > 1) {
        for (size_t idx_i = 0; idx_i < dim_m; idx_i++) {
            for (size_t idx_j = 0; idx_j < n; idx_j++) {
                uint32_t idx = ::genie::backend::get_mat_element(idx_mat, idx_i, idx_j);
                ::genie::backend::set_mat_element(likelihood_mat, idx_i, idx_j, ::genie::backend::get_arr_element(lut, idx));
            }
        }
    } else if (unique_count == 1) {
        uint32_t val = ::genie::backend::get_arr_element(lut, 0);
        for (size_t idx_i = 0; idx_i < dim_m; idx_i++) {
            for (size_t idx_j = 0; idx_j < n; idx_j++) {
                ::genie::backend::set_mat_element(likelihood_mat, idx_i, idx_j, val);
            }
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void serialize_mat_impl(UInt32MatDtype mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload) {
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
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
=======
    nrows = (uint32_t)::genie::backend::get_mat_shape(mat, 0);
    ncols = (uint32_t)::genie::backend::get_mat_shape(mat, 1);
    ::genie::backend::serialize_mat(mat, (uint32_t)dtype_id, nrows, ncols, payload);
}

void serialize_arr_impl(UInt32ArrDtype arr, uint32_t nelems, std::stringstream& payload) {
    ::genie::backend::serialize_arr(arr, nelems, payload);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
}

// ---------------------------------------------------------------------------------------------------------------------

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
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
=======
void encode_likelihood_impl(std::vector<core::record::VariantGenotype>& recs, LikelihoodParameters& params,
                       LikelihoodPayload& payload, size_t block_size, bool transform_flag) {
    EncodingOptions opt = {(uint32_t)block_size, transform_flag};
    LikelihoodEncodingBlock block;
    extract_likelihoods_impl(opt, block, recs);
    transform_likelihood_mat_impl(opt, block);
    ::genie::backend::serialize_mat(block.idx_mat, (uint32_t)block.dtype_id, block.nrows, block.ncols, block.serialized_mat);
    if (transform_flag) {
        ::genie::backend::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    }

>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
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

    params = LikelihoodParameters((uint8_t)recs.at(0).GetNumberOfLikelihoods(),
                                                   transform_flag, block.dtype_id);

    payload.setNRows(block.nrows);
    payload.setNCols(block.ncols);
    std::string str = block.serialized_mat.str();
    payload.setPayload(std::vector<uint8_t>(str.begin(), str.end()));

    if (transform_flag) {
        std::string arr_str = block.serialized_arr.str();
        payload.setAdditionalPayload(std::vector<uint8_t>(arr_str.begin(), arr_str.end()));
    }
}

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
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
=======
void decode_likelihood_impl(const LikelihoodParameters& params, LikelihoodPayload& payload,
                             std::vector<core::record::VariantGenotype>& recs) {
    EncodingOptions opt = {payload.getNRows(), params.GetTransformFlag()};
    LikelihoodEncodingBlock block;
    block.nrows = payload.getNRows();
    block.ncols = payload.getNCols();
    block.dtype_id = params.GetDtypeId();

    genie::entropy::lzma::LZMAEncoder lzmaDecoder;
    std::stringstream compressed, decompressed;

    if (!payload.getPayload().empty()) {
        std::vector<uint8_t> pl = payload.getPayload();
        compressed.write((const char*)pl.data(), pl.size());
        lzmaDecoder.decode(compressed, decompressed);
        std::string s = decompressed.str();
        std::vector<uint8_t> decomp_vec(s.begin(), s.end());
        ::genie::backend::deserialize_mat(decomp_vec, (uint32_t)block.dtype_id, block.nrows, block.ncols, block.idx_mat);
    }

    if (params.GetTransformFlag()) {
        std::vector<uint8_t> add_pl = payload.getAdditionalPayload();
        if (!add_pl.empty()) {
            compressed.str(""); compressed.clear();
            decompressed.str(""); decompressed.clear();
            compressed.write((const char*)add_pl.data(), add_pl.size());
            lzmaDecoder.decode(compressed, decompressed);
            std::string s = decompressed.str();
            std::vector<uint8_t> add_decomp_vec(s.begin(), s.end());
            block.nelems = (uint32_t)add_decomp_vec.size() / sizeof(uint32_t);
            ::genie::backend::deserialize_arr(add_decomp_vec, block.nelems, block.lut);
        }
    }

    inverse_transform_likelihood_mat_impl(opt, block);
    uint32_t gl_per_sample = params.GetNumGlPerSample();
    for (uint32_t idx_i = 0; idx_i < block.nrows; ++idx_i) {
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
        recs.emplace_back();
        std::vector<std::vector<uint32_t>> likelihoods;
        for (uint32_t idx_j = 0; idx_j < block.ncols; idx_j += gl_per_sample) {
            std::vector<uint32_t> sample_likelihoods;
<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
            for (uint8_t k = 0; k < params.GetNumGlPerSample(); ++k) {
                sample_likelihoods.push_back(block.likelihood_mat[i][j + k]);
=======
            for (uint8_t idx_k = 0; idx_k < gl_per_sample; ++idx_k) {
                sample_likelihoods.push_back(::genie::backend::get_mat_element(block.likelihood_mat, idx_i, idx_j + idx_k));
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc
            }
            likelihoods.push_back(sample_likelihoods);
        }
        recs.back().SetLikelihoods(std::move(likelihoods));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

<<<<<<< HEAD:src/genie/likelihood/likelihood_coder_xtensor.cc
}  // namespace genie::likelihood::detail::xtensor
=======
} // namespace genie::likelihood::detail
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions):src/genie/likelihood/likelihood_coder.cc

namespace genie::likelihood {

void encode_likelihood(std::vector<core::record::VariantGenotype>& recs,
                      LikelihoodParameters& params, LikelihoodPayload& payload,
                      size_t block_size, bool transform_flag) {
    detail::encode_likelihood_impl(recs, params, payload, block_size, transform_flag);
}

void decode_likelihood(const LikelihoodParameters& params, LikelihoodPayload& payload,
                      std::vector<core::record::VariantGenotype>& recs) {
    detail::decode_likelihood_impl(params, payload, recs);
}

void extract_likelihoods(const EncodingOptions& opt, LikelihoodEncodingBlock& block,
                        std::vector<core::record::VariantGenotype>& recs) {
    detail::extract_likelihoods_impl(opt, block, recs);
}

void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    detail::transform_likelihood_mat_impl(opt, block);
}

void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block) {
    detail::inverse_transform_likelihood_mat_impl(opt, block);
}

} // namespace genie::likelihood

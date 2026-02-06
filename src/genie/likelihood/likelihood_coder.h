/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

#include <cmath>
#include <tuple>
#include <sstream>
#include <vector>

#include "genie/core/variant_genotype_record/record.h"
#include "likelihood_parameters.h"
#include "likelihood_types.h"

// Backend Selection
#if defined(GENIE_LIKELIHOOD_BACKEND_XTENSOR)
    #include "likelihood_coder_xtensor.h"
    namespace genie::likelihood {
        namespace backend = detail::xtensor;
    }
#elif defined(GENIE_LIKELIHOOD_BACKEND_EIGEN)
    #include "likelihood_coder_eigen.h"
    namespace genie::likelihood {
        namespace backend = detail::eigen;
    }
#else // Default to STD
    #include "likelihood_coder_std.h"
    namespace genie::likelihood {
        namespace backend = detail::std_lib;
    }
#endif

namespace genie::likelihood {

class LikelihoodPayload;

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t block_size;
    bool transform_flag;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    uint32_t nrows;
    uint32_t ncols;
    UInt32MatDtype likelihood_mat;
    UInt32ArrDtype lut;
    uint32_t nelems;
    UInt32MatDtype idx_mat;
    core::DataType dtype_id = core::DataType::UINT32;
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;
    EncodingBlock()
        : nrows(0),
          ncols(0),
          likelihood_mat{},
          lut{},
          nelems(0),
          idx_mat{},
          dtype_id(core::DataType::UINT32),
          serialized_mat{},
          serialized_arr{} {}

    EncodingBlock(const EncodingBlock& other) {
        nrows = other.nrows;
        ncols = other.ncols;
        likelihood_mat = other.likelihood_mat;
        lut = other.lut;
        nelems = other.nelems;
        idx_mat = other.idx_mat;
        dtype_id = other.dtype_id;
        serialized_mat << other.serialized_mat.rdbuf();
        serialized_arr << other.serialized_arr.rdbuf();
    }
    EncodingBlock& operator=(const EncodingBlock& other) {
        nrows = other.nrows;
        ncols = other.ncols;
        likelihood_mat = other.likelihood_mat;
        lut = other.lut;
        nelems = other.nelems;
        idx_mat = other.idx_mat;
        dtype_id = other.dtype_id;
        serialized_mat << other.serialized_mat.rdbuf();
        serialized_arr << other.serialized_arr.rdbuf();
        return *this;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

inline void extract_likelihoods(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs
) {
    backend::extract_likelihoods(opt, block, recs);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    backend::transform_likelihood_mat(opt, block);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void inverse_transform_likelihood_mat(const EncodingOptions& opt, EncodingBlock& block) {
    backend::inverse_transform_likelihood_mat(opt, block);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id) {
    backend::transform_lut(likelihood_mat, lut, nelems, idx_mat, dtype_id);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    backend::inverse_transform_lut(likelihood_mat, lut, idx_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void serialize_mat(UInt32MatDtype mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload) {
    backend::serialize_mat(mat, dtype_id, nrows, ncols, payload);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void serialize_arr(UInt32ArrDtype arr, uint32_t nelems, std::stringstream& payload) {
    backend::serialize_arr(arr, nelems, payload);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void encode_likelihood(
    // Inputs
    std::vector<core::record::VariantGenotype>& recs,
    // Outputs
    LikelihoodParameters& params, LikelihoodPayload& payload,
    // Options
    size_t block_size = 512, bool transform_flag = true) {
    backend::encode_likelihood(recs, params, payload, block_size, transform_flag);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void decode_likelihood(
    // Inputs
    const LikelihoodParameters& params, LikelihoodPayload& payload,
    // Outputs
    std::vector<core::record::VariantGenotype>& recs) {
    backend::decode_likelihood(params, payload, recs);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::likelihood

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

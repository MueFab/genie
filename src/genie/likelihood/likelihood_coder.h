/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
#define GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H

<<<<<<< HEAD
#include <cmath>
#include <tuple>
#include <sstream>
#include <vector>

=======
#include <vector>
#include <sstream>
#include <genie/backend/backend.h>
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)
#include "genie/core/variant_genotype_record/record.h"
#include "likelihood_parameters.h"
#include "likelihood_types.h"

<<<<<<< HEAD
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
=======
namespace genie::likelihood {
    class LikelihoodPayload;

    struct EncodingOptions {
        uint32_t block_size;
        bool transform_flag;
    };

    namespace detail {
        struct LikelihoodEncodingBlock;
    }
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

    // Compatibility aliases/facades for tests
    using LikelihoodEncodingBlock = detail::LikelihoodEncodingBlock;

    void encode_likelihood(
        std::vector<core::record::VariantGenotype>& recs,
        LikelihoodParameters& params, LikelihoodPayload& payload,
        size_t block_size = 512, bool transform_flag = true);

<<<<<<< HEAD
struct EncodingOptions {
    uint32_t block_size;
    bool transform_flag;
};
=======
    void decode_likelihood(
        const LikelihoodParameters& params, LikelihoodPayload& payload,
        std::vector<core::record::VariantGenotype>& recs);

    void extract_likelihoods(
        const EncodingOptions& opt,
        LikelihoodEncodingBlock& block,
        std::vector<core::record::VariantGenotype>& recs);

    void transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

    void inverse_transform_likelihood_mat(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

}

namespace genie::likelihood::detail {

// Unified Matrix Types from Backend
using UInt32ArrDtype = ::genie::backend::UIntVecDtype;
using UInt32MatDtype = ::genie::backend::UIntMatDtype;

struct LikelihoodEncodingBlock {
    uint32_t nrows;
    uint32_t ncols;
    
    // Generic elements
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;
    
    // Backend specific (Aliased)
    UInt32MatDtype likelihood_mat;
    UInt32ArrDtype lut;
    uint32_t nelems;
    UInt32MatDtype idx_mat;
    core::DataType dtype_id = core::DataType::UINT32;

    LikelihoodEncodingBlock() : nrows(0), ncols(0), nelems(0) {}
};

<<<<<<< HEAD
// ---------------------------------------------------------------------------------------------------------------------

inline void extract_likelihoods(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs
) {
    backend::extract_likelihoods(opt, block, recs);
}
=======
// Unified Implementation Signatures
void extract_likelihoods_impl(
    const EncodingOptions& opt,
    LikelihoodEncodingBlock& block,
    std::vector<core::record::VariantGenotype>& recs);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

void transform_likelihood_mat_impl(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

<<<<<<< HEAD
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
=======
void inverse_transform_likelihood_mat_impl(const EncodingOptions& opt, LikelihoodEncodingBlock& block);

void transform_lut_impl(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, uint32_t& nelems, UInt32MatDtype& idx_mat,
                   core::DataType& dtype_id);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

void inverse_transform_lut_impl(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat);

<<<<<<< HEAD
inline void inverse_transform_lut(UInt32MatDtype& likelihood_mat, UInt32ArrDtype& lut, UInt32MatDtype& idx_mat) {
    backend::inverse_transform_lut(likelihood_mat, lut, idx_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void serialize_mat(UInt32MatDtype mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload) {
    backend::serialize_mat(mat, dtype_id, nrows, ncols, payload);
}
=======
void serialize_mat_impl(UInt32MatDtype mat, core::DataType dtype_id, uint32_t& nrows, uint32_t& ncols,
                   std::stringstream& payload);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

void serialize_arr_impl(UInt32ArrDtype arr, uint32_t nelems, std::stringstream& payload);

<<<<<<< HEAD
inline void serialize_arr(UInt32ArrDtype arr, uint32_t nelems, std::stringstream& payload) {
    backend::serialize_arr(arr, nelems, payload);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void encode_likelihood(
    // Inputs
=======
void encode_likelihood_impl(
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)
    std::vector<core::record::VariantGenotype>& recs,
    LikelihoodParameters& params, LikelihoodPayload& payload,
<<<<<<< HEAD
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
=======
    size_t block_size = 512, bool transform_flag = true);

void decode_likelihood_impl(
    const LikelihoodParameters& params, LikelihoodPayload& payload,
    std::vector<core::record::VariantGenotype>& recs);
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

} // namespace genie::likelihood::detail

<<<<<<< HEAD
}  // namespace genie::likelihood

#endif  // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
=======
#endif // GENIE_LIKELIHOOD_LIKELIHOOD_CODER_H
>>>>>>> 8577ee64 (refactor(test): standardize infrastructure, resolve linker dependencies, and align naming conventions)

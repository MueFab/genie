/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONTACT_CODER_H
#define GENIE_CONTACT_CONTACT_CODER_H

#include <cstdint>
#include <list>
#include <tuple>
#include <optional>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "contact_parameters.h"

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using UInt8VecDtype = xt::xtensor<uint8_t , 1, xt::layout_type::row_major>;
//using UInt8MatDtype = xt::xtensor<uint8_t , 2, xt::layout_type::row_major>;
//using Int8MatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
using UIntMatDtype = xt::xtensor<uint32_t , 2, xt::layout_type::row_major>;
using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;

using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

//using ChrIDPair = std::pair<uint8_t, uint8_t>;
using SCMRecDtype = std::unordered_map<ChrIDPair, core::record::ContactRecord, chr_pair_hash>;
using IntervSCMRecDtype = std::unordered_map<uint32_t, SCMRecDtype>;

// ---------------------------------------------------------------------------------------------------------------------

enum class SortingAlgoID : uint8_t {
    NO_SORTING = 0,
    RANDOM_SORT = 1,
    NEAREST_NEIGHBOR = 2,
    LIN_KERNIGHAN_HEURISTIC = 3,
    UNDEFINED = 4
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t tile_size;
    bool multi_intervals = false;
    bool diag_transform = true;
    bool binarize = true;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    ContactParameters params;
    IntervSCMRecDtype interv_scm_recs;
};

// ---------------------------------------------------------------------------------------------------------------------

void decompose(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::ContactRecord>& recs);

// ---------------------------------------------------------------------------------------------------------------------

void compute_mask(UInt64VecDtype& ids,BinVecDtype& mask);

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(UInt64VecDtype& row_ids,UInt64VecDtype& col_ids,bool is_intra,
                   BinVecDtype& row_mask,BinVecDtype& col_mask);

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, bool is_intra,
                      BinVecDtype& row_mask, BinVecDtype& col_mask);

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts,
    UIntMatDtype& mat, size_t nrows, size_t ncols,
    uint64_t row_id_offset=0, uint64_t col_id_offset=0);

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(UIntMatDtype& mat, uint64_t row_id_offset, uint64_t col_id_offset,
                     UInt64VecDtype& row_ids, UInt64VecDtype& col_ids,UIntVecDtype& counts);

// ---------------------------------------------------------------------------------------------------------------------

void sort_by_row_ids(UInt64VecDtype& row_ids,UInt64VecDtype& col_ids,UIntVecDtype& counts);

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    UIntMatDtype& mat,
    uint8_t mode
);

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(ContactParameters& params, core::record::ContactRecord& rec);

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm(const EncodingOptions& opt, EncodingBlock& block);

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<ContactParameters, EncodingBlock> encode_block(const EncodingOptions& opt,
                                                           std::vector<core::record::ContactRecord>& recs);

// ---------------------------------------------------------------------------------------------------------------------

}
}

#endif  // GENIE_CONTACT_CONTACT_CODER_H

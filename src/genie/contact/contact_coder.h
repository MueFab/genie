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
using UInt8MatDtype = xt::xtensor<uint8_t , 2, xt::layout_type::row_major>;
using Int8MatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;

using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

//using ChrIDPair = std::pair<uint8_t, uint8_t>;
using SCMRecDtype = std::unordered_map<ChrIDPair, core::record::ContactRecord, chr_pair_hash>;
using IntervSCMRecDtype = std::unordered_map<uint32_t, SCMRecDtype>;

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

std::tuple<ContactParameters, EncodingBlock> encode_block(const EncodingOptions& opt,
                                                           std::vector<core::record::ContactRecord>& recs);

// ---------------------------------------------------------------------------------------------------------------------

}
}

#endif  // GENIE_CONTACT_CONTACT_CODER_H

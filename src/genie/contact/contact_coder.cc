/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime-exception.h>
#include "xtensor/xsort.hpp"
#include "contact_parameters.h"
#include "contact_subcm_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::ContactRecord>& recs
){
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto& params = block.params;
    auto& interv_scm_recs = block.interv_scm_recs;

    bool sample_parsed = false;
    bool interv_parsed = false;

    for (auto& rec: recs){
        auto sample_ID = rec.getSampleID();
        auto sample_name = rec.getSampleName();

        // TODO (Yeremia): Extend to multi-samples
        if (sample_parsed) {
            auto& samples = params.getSamples();
            UTILS_DIE_IF(samples.find(sample_ID) != samples.end(), "sample_ID already exists!");
        } else {
            params.addSample(sample_ID, std::move(sample_name));
            sample_parsed = true;
        }

        auto chr1_ID = rec.getChr1ID();
        auto chr2_ID = rec.getChr2ID();
        params.upsertChromosome(
            chr1_ID,
            rec.getChr1Name(),
            rec.inferChr1Length()
        );

        params.upsertChromosome(
            chr2_ID,
            rec.getChr2Name(),
            rec.inferChr2Length()
        );

        // TODO (Yeremia): Extend to norm_methods and norm_mat

        // TODO (Yeremia): Extend to multi-intervals
        auto rec_interval = rec.inferInterval();
        auto chr_pair = ChrIDPair(chr1_ID, chr2_ID);
        if (interv_parsed){
            if (opt.multi_intervals){
                UTILS_DIE("Not implemented error!");
            } else {
                UTILS_DIE_IF(params.getInterval() != rec.inferInterval(), "Interval differs");
            }
        } else {
            params.setInterval(rec_interval);
            auto scm_rec = SCMRecDtype();
            scm_rec.emplace(chr_pair, std::move(rec));
            interv_scm_recs.emplace(rec_interval, std::move(scm_rec));
            interv_parsed = true;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BinVecDtype&& compute_mask(UInt64VecDtype& ids){
    BinVecDtype mask = BinVecDtype({xt::amax(ids)});
    UInt64VecDtype unique_ids = xt::unique(ids);
    for (auto id: unique_ids){
        mask(id) = true;
    }

    return std::move(mask);
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<BinVecDtype, BinVecDtype>&& compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra
){
    auto row_mask = BinVecDtype(compute_mask(row_ids));
    auto col_mask = BinVecDtype(compute_mask(col_ids));

    if (is_intra){
        BinVecDtype mask = row_mask || col_mask;
        xt::view(row_mask, xt::all()) = xt::view(mask, xt::all());
        xt::view(col_mask, xt::all()) = xt::view(mask, xt::all());
    }

    std::pair<BinVecDtype, BinVecDtype> mask_pair = std::make_pair(std::move(row_mask), std::move(col_mask));

    return std::move(mask_pair);
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    ContactParameters& params,
    core::record::ContactRecord& rec
) {
    auto interval = params.getInterval();
    auto num_counts = rec.getNumCounts();

    auto chr1_ID = rec.getChr1ID();
//    auto chr1_nbins = params.getNumBinEntries(chr1_ID);
    auto chr1_ntiles = params.getNumTiles(chr1_ID);

    auto chr2_ID = rec.getChr2ID();
//    auto chr2_nbins = params.getNumBinEntries(chr2_ID);
    auto chr2_ntiles = params.getNumTiles(chr2_ID);

    auto is_intra = chr1_ID == chr2_ID;

    UInt64VecDtype row_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    row_ids /= interval;

    UInt64VecDtype col_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    col_ids /= interval;

    UIntVecDtype counts = xt::adapt(rec.getCounts(), {num_counts});

    auto mask_pair = std::pair<BinVecDtype, BinVecDtype>(compute_masks(row_ids, col_ids, is_intra));
//    auto& row_mask = mask_pair.first;
//    auto& col_mask = mask_pair.second;

//    UInt64VecDtype unique_ids = xt::unique(xt::concatenate(xtuple(row_ids, col_ids), 0));
////    UInt64VecDtype mapping = xt::zeros<uint64_t>(counts.shape());
////    size_t new_id = 0;
////    for (auto unique_id: unique_ids){
////        mapping(unique_id) = new_id++;
////    }

    for (size_t i_tile=0; i_tile<chr1_ntiles; i_tile++){
        auto min_row_id = i_tile*interval;
        auto max_row_id = (i_tile+1)*interval;

        for (size_t j_tile=0; j_tile<chr2_ntiles; j_tile++){
            auto min_col_id = j_tile*interval;
            auto max_col_id = (j_tile+1)*interval;

            BinVecDtype mask1 = (row_ids >= min_row_id) && (row_ids < max_row_id);
            BinVecDtype mask2 = (col_ids >= min_col_id) && (col_ids < max_col_id);

        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm(
    const EncodingOptions& opt,
    EncodingBlock& block
) {
    auto& params = block.params;
    auto& interv_scm_recs = block.interv_scm_recs;

    auto interval = params.getInterval();
    auto& scm_recs = interv_scm_recs[interval];

    UInt8VecDtype sorted_chr_IDs = {params.getNumberChromosomes()};
    size_t i = 0;
    for (auto& chr_info: params.getChromosomes()){
        sorted_chr_IDs(i) = chr_info.second.ID;
    }

    sorted_chr_IDs = xt::sort(sorted_chr_IDs);

    for (uint8_t chr1_ID: sorted_chr_IDs){
        for (uint8_t chr2_ID: sorted_chr_IDs){
            if (chr1_ID > chr2_ID){
                continue;
            }

            auto chr_pair = ChrIDPair(chr1_ID, chr2_ID);
            auto& rec = scm_recs[chr_pair];

            encode_scm(params, rec);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
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

void compute_mask(
    UInt64VecDtype& ids,
    // Output
    BinVecDtype& mask
){
    auto nelems = xt::amax(ids)(0)+1;
//    mask.resize({nelems});
    mask = xt::zeros<bool>({nelems});

    UInt64VecDtype unique_ids = xt::unique(ids);
    for (auto id: unique_ids){
        mask(id) = true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    const bool is_intra,
    // Outputs:
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    UTILS_DIE_IF(row_ids.shape(0) != col_ids.shape(0),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra){
        BinVecDtype mask;
        UInt64VecDtype ids = xt::concatenate(xt::xtuple(row_ids, col_ids));
        compute_mask(ids, mask);

        row_mask.resize(mask.shape());
        row_mask = mask;
        col_mask.resize(mask.shape());
        col_mask = mask;
    } else {
        compute_mask(row_ids, row_mask);
        compute_mask(col_ids, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    UTILS_DIE_IF(row_ids.shape(0) != col_ids.shape(0),
                 "The size of row_ids and col_ids must be same!");

    auto num_entries = row_ids.shape(0);

    if (is_intra){
        UTILS_DIE_IF(row_mask.shape(0) != col_mask.shape(0), "Invalid mask!");
        // Does not matter either row_mask or col_mask
        auto& mask = row_mask;
        auto mapping = xt::cumsum(xt::cast<uint64_t>(mask)) - 1u;

        for (auto i = 0u; i<num_entries; i++){
            auto old_id = row_ids(i);
            auto new_id = mapping(old_id);
            row_ids(i) = new_id;
        }
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = col_ids(i);
            auto new_id = mapping(old_id);
            col_ids(i) = new_id;
        }
    } else {
        auto row_mapping = xt::cumsum(xt::cast<uint64_t>(row_mask)) - 1u;
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = row_ids(i);
            auto new_id = row_mapping(old_id);
            row_ids(i) = new_id;
        }
        auto col_mapping = xt::cumsum(xt::cast<uint64_t>(col_mask)) - 1u;
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = col_ids(i);
            auto new_id = col_mapping(old_id);
            col_ids(i) = new_id;
        }
    }

}

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts,
    UIntMatDtype& mat,
    size_t nrows,
    size_t ncols,
    uint64_t row_id_offset,
    uint64_t col_id_offset
){
//    mat = UIntMatDtype({nrows, ncols});
    mat = xt::zeros<uint32_t>({nrows, ncols});

    row_ids -= row_id_offset;
    col_ids -= col_id_offset;

    auto nentries = counts.size();
    for (auto i = 0u; i<nentries; i++){
        auto row_id = row_ids(i);
        auto col_id = col_ids(i);
        auto count = counts(i);
        mat(row_id, col_id) = count;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diagonal_transform_scm(
    UIntMatDtype& scm_at
){

}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    ContactParameters& params,
    core::record::ContactRecord& rec
) {

    // Initialize variables
    BinVecDtype row_mask;
    BinVecDtype col_mask;

    auto interval = params.getInterval();
    auto num_counts = rec.getNumCounts();

    auto chr1_ID = rec.getChr1ID();
    auto chr1_nbins = params.getNumBinEntries(chr1_ID);
    auto chr1_ntiles = params.getNumTiles(chr1_ID);

    auto chr2_ID = rec.getChr2ID();
    auto chr2_nbins = params.getNumBinEntries(chr2_ID);
    auto chr2_ntiles = params.getNumTiles(chr2_ID);

    auto is_intra = chr1_ID == chr2_ID;

    UInt64VecDtype row_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    row_ids /= interval;

    UInt64VecDtype col_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    col_ids /= interval;

    UIntVecDtype counts = xt::adapt(rec.getCounts(), {num_counts});

    // Compute mask
    compute_masks(row_ids, col_ids, is_intra, row_mask, col_mask);

    // Create mapping
    remove_unaligned(row_ids, col_ids, is_intra, row_mask, col_mask);

    for (size_t i_tile=0; i_tile<chr1_ntiles; i_tile++){
        auto min_row_id = i_tile*interval;
        auto max_row_id = (i_tile+1)*interval;

        for (size_t j_tile=0; j_tile<chr2_ntiles; j_tile++){
            auto min_col_id = j_tile*interval;
            auto max_col_id = (j_tile+1)*interval;

            BinVecDtype mask1 = (row_ids >= min_row_id) && (row_ids < max_row_id);
            BinVecDtype mask2 = (col_ids >= min_col_id) && (col_ids < max_col_id);
            BinVecDtype mask = mask1 && mask2;

            UInt64VecDtype tile_row_ids = xt::filter(row_ids, mask);
            UInt64VecDtype tile_col_ids = xt::filter(col_ids, mask);
            UIntVecDtype tile_counts = xt::filter(counts, mask);

            auto nrows = std::max(max_row_id, chr1_nbins) - min_row_id;
            auto ncols = std::max(max_col_id, chr2_nbins) - min_col_id;

            UIntMatDtype tile_mat;
            sparse_to_dense(
                tile_row_ids,
                tile_col_ids,
                tile_counts,
                tile_mat,
                nrows,
                ncols,
                min_row_id,
                min_col_id
            );
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
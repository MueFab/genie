/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime-exception.h>
#include "contact_parameters.h"
#include "subcontact_matrix_parameters.h"
#include "xtensor/xsort.hpp"

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
        auto chr1_name = std::string(rec.getChr1Name());
        params.upsertChromosome(chr1_ID, std::move(chr1_name), rec.inferChr1Length());

        auto chr2_ID = rec.getChr2ID();
        auto chr2_name = std::string(rec.getChr2Name());
        params.upsertChromosome(chr2_ID, std::move(chr2_name), rec.inferChr2Length());

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

void append_unaligned(
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
        auto tmp_ids = xt::argwhere(mask);

        auto mapping_len = xt::sum(xt::cast<uint64_t>(mask))(0);
        auto mapping = xt::empty<uint64_t>({mapping_len});

        auto k = 0u;
        for (auto v : tmp_ids){
            auto value = v[0];
            mapping(k++) = value;
        }

        for (auto i = 0u; i<num_entries; i++){
            row_ids(i) = mapping(row_ids(i));
            col_ids(i) = mapping(col_ids(i));
        }

    } else {
        auto tmp_ids = xt::argwhere(row_mask);
        auto mapping_len = xt::sum(xt::cast<uint64_t>(row_mask))(0);
        auto mapping = xt::empty<uint64_t>({mapping_len});

        auto k = 0u;
        for (auto v : tmp_ids){
            auto value = v[0];
            mapping(k++) = value;
        }

        for (auto i = 0u; i<num_entries; i++){
            row_ids(i) = mapping(row_ids(i));
        }

        tmp_ids = xt::argwhere(col_mask);
        mapping_len = xt::sum(xt::cast<uint64_t>(col_mask))(0);
        mapping = xt::empty<uint64_t>({mapping_len});

        k = 0u;
        for (auto v : tmp_ids){
            auto value = v[0];
            mapping(k++) = value;
        }

        for (auto i = 0u; i<num_entries; i++){
            col_ids(i) = mapping(col_ids(i));
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
    mat = xt::zeros<uint32_t>({nrows, ncols});

    auto nentries = counts.size();
    for (auto i = 0u; i<nentries; i++){
        auto row_id = row_ids(i) - row_id_offset;
        auto col_id = col_ids(i) - col_id_offset;
        auto count = counts(i);
        mat(row_id, col_id) = count;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    UIntMatDtype& mat,
    uint64_t row_id_offset,
    uint64_t col_id_offset,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    BinMatDtype mask = mat > 0u;

    auto ids = xt::argwhere(mask);
    auto num_entries = ids.size();

    row_ids.resize({num_entries});
    col_ids.resize({num_entries});
    counts.resize({num_entries});

    for (auto k = 0u; k< num_entries; k++){
        auto i = ids[k][0];
        auto j = ids[k][1];
        auto c = mat(i,j);

        row_ids[k] = i;
        col_ids[k] = j;
        counts[k] = c;
    }

    row_ids += row_id_offset;
    col_ids += col_id_offset;
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    auto num_entries = row_ids.size();

    UInt64VecDtype sort_ids = xt::argsort(row_ids);
    UInt64VecDtype tmp_row_ids = row_ids;
    UInt64VecDtype tmp_col_ids = col_ids;
    UIntVecDtype tmp_counts = counts;

    for (auto k = 0u; k< num_entries; k++){
        tmp_row_ids(k) = row_ids(sort_ids(k));
        tmp_col_ids(k) = col_ids(sort_ids(k));
        tmp_counts(k) = counts(sort_ids(k));
    }

    row_ids = tmp_row_ids;
    col_ids = tmp_col_ids;
    counts = tmp_counts;
}


// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    UIntMatDtype trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(mat.shape(0) != mat.shape(1), "Matrix must be a square!");

        auto n = mat.shape(0);
        auto new_nrows = n / 2 + 1;
        trans_mat = xt::zeros<uint32_t>({new_nrows, n});

        auto o = 0u;
        for (auto k = 0u; k < n; k++) {
            for (auto i = 0u; i < (n - k); i++) {
                auto j = i + k;

                auto v = mat(i, j);
                if (v != 0) {
                    auto new_i = o / n;
                    auto new_j = o % n;
                    trans_mat(new_i, new_j) = v;
                }
                o++;
            }
        }
        mat.resize(trans_mat.shape());
        mat = trans_mat;

    } else {
        auto nrows = static_cast<int64_t>(mat.shape(0));
        auto ncols = static_cast<int64_t>(mat.shape(1));
        trans_mat = xt::zeros<uint32_t>({nrows, ncols});

        Int64VecDtype diag_ids = xt::empty<int64_t>({nrows+ncols-1});
        size_t k_elem;

        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids(0) = 0;
            k_elem = 1u;
            auto ndiags = std::max(nrows, ncols);
            for (auto diag_id = 1; diag_id<ndiags; diag_id++){
                if (diag_id < static_cast<int64_t>(ncols))
                    diag_ids(k_elem++) = diag_id;
                if (diag_id < static_cast<int64_t>(nrows))
                    diag_ids(k_elem++) = -diag_id;
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            k_elem = 0u;
            for (int64_t diag_id = -nrows+1; diag_id < ncols; diag_id++)
                diag_ids(k_elem++) = diag_id;
        } else if (mode == DiagonalTransformMode::MODE_3){
            k_elem = 0u;
            for (int64_t diag_id = ncols-1; diag_id > -nrows; diag_id--)
                diag_ids(k_elem++) = diag_id;
        }

        int64_t i;
        int64_t j;
        int64_t i_offset;
        int64_t j_offset;
        int64_t nelems_in_diag;
        auto o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max(nrows, ncols) - diag_id;
                i_offset = 0;
                j_offset = diag_id;
            } else {
                nelems_in_diag = std::max(nrows, ncols) + diag_id;
                i_offset = -diag_id;
                j_offset = 0;
            }
            for (auto k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i = k_diag + i_offset;
                j = k_diag + j_offset;
                if (i >= nrows)
                    break;
                if (j >= ncols)
                    break;

                auto v = mat(i, j);
                if (v != 0) {
                    size_t new_i = o / mat.shape(1);
                    size_t new_j = o % mat.shape(1);
                    trans_mat(new_i, new_j) = v;
                }
                o++;
            }
        }
        mat = trans_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void binarize_rows(
    UIntMatDtype& mat,
    BinMatDtype& bin_mat
) {
    auto nrows = mat.shape(0);
    auto ncols = mat.shape(1);

    auto nbits_per_row = xt::cast<uint8_t>(xt::ceil(xt::log2(xt::amax(mat, {1}) + 1)));

    uint64_t bin_mat_nrows = static_cast<uint64_t>(xt::sum(nbits_per_row)(0));
    uint64_t bin_mat_ncols = ncols + 1;

    bin_mat = xt::zeros<bool>({bin_mat_nrows, bin_mat_ncols});

    size_t i2 = 0;
    for (size_t i = 0; i < nrows; i++) {
        for (size_t i_bit = 0; i_bit < nbits_per_row[i]; i_bit++) {
            xt::view(bin_mat, i2++, xt::range(1, bin_mat_ncols)) = xt::cast<bool>(xt::view(mat, i, xt::all()) & (1u << i_bit));
        }
        bin_mat(i2-1, 0) = true;
    }
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
    std::vector<genie::core::record::ContactRecord>& recs,
    const EncodingOptions& opt,
    EncodingBlock& block
) {

    auto& params = block.params;
    auto& interv_scm_recs = block.interv_scm_recs;

//    auto& interval = params.getInterval();
//    auto& scm_recs = interv_scm_recs[interval];

    uint32_t interval = 0;
    std::map<uint8_t, SampleInformation> samples;
    std::map<uint8_t, ChromosomeInformation> chrs;
    for (auto& rec: recs){
        auto rec_interval = rec.inferInterval();
        if (interval == 0)
            interval = rec_interval;
        else
            UTILS_DIE_IF(interval != rec_interval, "records has different interval");

        {
            auto sample_name = std::string(rec.getSampleName());
            params.addSample(rec.getSampleID(), std::move(sample_name));
            auto chr1_name = std::string(rec.getChr1Name());
            params.upsertChromosome(rec.getChr1ID(), std::move(chr1_name), rec.inferChr1Length());
            auto chr2_name = std::string(rec.getChr2Name());
            params.upsertChromosome(rec.getChr2ID(), std::move(chr2_name), rec.inferChr2Length());
        }

    }

    UInt8VecDtype sorted_chr_IDs = {params.getNumberChromosomes()};
    size_t i = 0;
    for (auto& chr_info: params.getChromosomes()){
        sorted_chr_IDs(i) = chr_info.second.ID;
    }

    sorted_chr_IDs = xt::sort(sorted_chr_IDs);

    for (uint8_t chr1_ID: sorted_chr_IDs){
        for (uint8_t chr2_ID: sorted_chr_IDs){

            // Store only the upper triangle part of the contact matrix
            if (chr1_ID > chr2_ID){
                continue;
            }

            auto chr_pair = ChrIDPair(chr1_ID, chr2_ID);
//            auto& rec = scm_recs[chr_pair];

//            encode_scm(params, rec);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<ContactParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::ContactRecord>& recs){


}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
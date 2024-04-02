/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime-exception.h>
#include <xtensor/xsort.hpp>
#include <codecs/include/mpegg-codecs.h>
#include "contact_parameters.h"
#include "subcontact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

//void decompose(
//    const EncodingOptions& opt,
//    EncodingBlock& block,
//    std::vector<core::record::ContactRecord>& recs
//){
//    UTILS_DIE_IF(recs.empty(), "No records found for the process!");
//
//    auto& params = block.params;
//    auto& interv_scm_recs = block.interv_scm_recs;
//
//    bool sample_parsed = false;
//    bool interv_parsed = false;
//
//    for (auto& rec: recs){
//        auto sample_ID = rec.getSampleID();
//        auto sample_name = rec.getSampleName();
//
//        // TODO (Yeremia): Extend to multi-samples
//        if (sample_parsed) {
//            auto& samples = params.getSamples();
//            UTILS_DIE_IF(samples.find(sample_ID) != samples.end(), "sample_ID already exists!");
//        } else {
//            params.addSample(sample_ID, std::move(sample_name));
//            sample_parsed = true;
//        }
//
//        auto chr1_ID = rec.getChr1ID();
//        auto chr1_name = std::string(rec.getChr1Name());
//        params.upsertChromosome(chr1_ID, std::move(chr1_name), rec.getChr2Length());
//
//        auto chr2_ID = rec.getChr2ID();
//        auto chr2_name = std::string(rec.getChr2Name());
//        params.upsertChromosome(chr2_ID, std::move(chr2_name), rec.getChr2Length());
//
//        // TODO (Yeremia): Extend to norm_methods and norm_mat
//
//        // TODO (Yeremia): Extend to multi-intervals
//        auto rec_interval = rec.getBinSize();
//        auto chr_pair = ChrIDPair(chr1_ID, chr2_ID);
//        if (interv_parsed){
//            if (opt.multi_intervals){
//                UTILS_DIE("Not implemented error!");
//            } else {
//                UTILS_DIE_IF(params.getBinSize() != rec.getBinSize(), "Interval differs");
//            }
//        } else {
//            params.setBinSize(rec_interval);
//            auto scm_rec = SCMRecDtype();
//            scm_rec.emplace(chr_pair, std::move(rec));
//            interv_scm_recs.emplace(rec_interval, std::move(scm_rec));
//            interv_parsed = true;
//        }
//    }
//}

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
    auto dim = mat.shape(0);
    if (mat.shape(0) == 0 || mat.shape(1) == 0)
        mat = xt::zeros<uint32_t>({nrows, ncols});
    else
        mat.resize({nrows, ncols});

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

    if (mode == DiagonalTransformMode::NONE){
        return ; // Do nothing
    } else if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(mat.shape(0) != mat.shape(1), "Matrix must be a square!");

        auto n = mat.shape(0);
        auto new_nrows = n / 2 + 1;
        trans_mat = xt::zeros<uint32_t>({new_nrows, n});

        auto o = 0u;
        for (size_t k = 0u; k < n; k++) {
            for (size_t i = 0u; i < (n - k); i++) {
                size_t j = i + k;

                auto v = mat(i, j);
                if (v != 0) {
                    size_t new_i = o / n;
                    size_t new_j = o % n;
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

        int64_t i, j;
        int64_t i_offset, j_offset;
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

void debinarize_row_bin(BinMatDtype& bin_mat, UIntMatDtype& mat){
    UTILS_DIE("Not yet implemented!");
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO(yeremia): completely the same as the function with same name in genotype. Move this to somewhere elsee
void bin_mat_to_bytes(BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len) {
    auto nrows = static_cast<size_t>(bin_mat.shape(0));
    auto ncols = static_cast<size_t>(bin_mat.shape(1));

    auto bpl = (ncols >> 3) + ((ncols & 7) > 0);  // Ceil operation
    payload_len = bpl * nrows;
    *payload = (unsigned char*)calloc(payload_len, sizeof(unsigned char));

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3);
            uint8_t shift = (7 - (j & 7));
            auto val = static_cast<uint8_t>(bin_mat(i, j));
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }

    // TODO(yeremia): find a better solution to free the memory of bin_mat
    bin_mat.resize({0,0});
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO(yeremia): completely the same as the function with same name in genotype. Move this to somewhere elsee
void bin_mat_from_bytes(BinMatDtype& bin_mat, const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols) {
    auto bpl = (ncols >> 3) + ((ncols & 7) > 0);  // bytes per line with ceil operation
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len / nrows / ncols!");

    MatShapeDtype bin_mat_shape = {nrows, ncols};
    bin_mat.resize(bin_mat_shape);
    xt::view(bin_mat, xt::all(), xt::all()) = false;  // Initialize value with 0

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3);
            uint8_t shift = (7 - (j & 7));
            bin_mat(i, j) = (*(payload + byte_offset) >> shift) & 1;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm(
    genie::contact::SubcontactMatrixPayload& scm_payload,
    ContactMatrixParameters& params,
    core::record::ContactRecord& rec
){

}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    const EncodingOptions& opt,
    ContactMatrixParameters& params,
    core::record::ContactRecord& rec,
    genie::contact::SubcontactMatrixPayload& scm_payload
) {

    // Initialize variables
    BinVecDtype row_mask;
    BinVecDtype col_mask;

    auto interval = params.getBinSize();
    auto num_counts = rec.getNumCounts();

    auto chr1_ID = scm_payload.getChr1ID();
    auto chr1_nbins = params.getNumBinEntries(chr1_ID);
    auto chr1_ntiles = params.getNumTiles(chr1_ID);

    auto chr2_ID = scm_payload.getChr2ID();
    auto chr2_nbins = params.getNumBinEntries(chr2_ID);
    auto chr2_ntiles = params.getNumTiles(chr2_ID);

    auto is_intra_scm = chr1_ID == chr2_ID;

    UInt64VecDtype row_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    row_ids /= interval;

    UInt64VecDtype col_ids = xt::adapt(rec.getStartPos1(), {num_counts});
    col_ids /= interval;

    UIntVecDtype counts = xt::adapt(rec.getCounts(), {num_counts});

    // Compute mask
    compute_masks(row_ids, col_ids, is_intra_scm, row_mask, col_mask);

    // Create mapping
    remove_unaligned(row_ids, col_ids, is_intra_scm, row_mask, col_mask);

    std::list<ContactMatrixTilePayload> tile_payloads;

    for (size_t i_tile=0; i_tile<chr1_ntiles; i_tile++){
        auto min_row_id = i_tile*opt.tile_size;
        auto max_row_id = (i_tile+1)*opt.tile_size;

        for (size_t j_tile=0; j_tile<chr2_ntiles; j_tile++){
            auto min_col_id = j_tile*opt.tile_size;
            auto max_col_id = (j_tile+1)*opt.tile_size;

            BinVecDtype mask1 = (row_ids >= min_row_id) && (row_ids < max_row_id);
            BinVecDtype mask2 = (col_ids >= min_col_id) && (col_ids < max_col_id);
            BinVecDtype mask = mask1 && mask2;

            UInt64VecDtype tile_row_ids = xt::filter(row_ids, mask);
            UInt64VecDtype tile_col_ids = xt::filter(col_ids, mask);
            UIntVecDtype tile_counts = xt::filter(counts, mask);

            auto tile_nrows = std::min(max_row_id, chr1_nbins) - min_row_id;
            auto tile_ncols = std::min(max_col_id, chr2_nbins) - min_col_id;

            UIntMatDtype tile_mat;
            sparse_to_dense(
                tile_row_ids,
                tile_col_ids,
                tile_counts,
                tile_mat,
                tile_nrows,
                tile_ncols,
                min_row_id,
                min_col_id
            );

            auto mode = DiagonalTransformMode::NONE;
            // Hardcoded option for the chosen mode
            if (opt.diag_transform) {
                if (is_intra_scm && i_tile == j_tile)
                    mode = DiagonalTransformMode::MODE_0;
                else if (i_tile < j_tile)
                    mode = DiagonalTransformMode::MODE_1;
                else if (i_tile > j_tile)
                    mode = DiagonalTransformMode::MODE_2;
            }

            genie::contact::diag_transform(tile_mat, mode);

            if (opt.binarize){
                genie::contact::BinMatDtype bin_mat;
                genie::contact::binarize_rows(tile_mat, bin_mat);

                if (opt.codec == genie::core::AlgoID::JBIG) {
                    uint8_t* payload;
                    size_t payload_len;
                    uint8_t* compressed_payload;
                    size_t compressed_payload_len;

                    size_t bin_mat_nrows = bin_mat.shape(0);
                    size_t bin_mat_ncols = bin_mat.shape(1);

                    bin_mat_to_bytes(bin_mat, &payload, payload_len);

                    mpegg_jbig_compress_default(
                        &compressed_payload,
                        &compressed_payload_len,
                        payload,
                        payload_len,
                        bin_mat_nrows,
                        bin_mat_ncols
                    );

                    tile_payloads.emplace_back(
                        opt.codec,
                        bin_mat_nrows,
                        bin_mat_ncols,
                        &compressed_payload,
                        compressed_payload_len
                    );
                } else {
                    UTILS_DIE("Not yet implemented for other codec!");
                }
            } else {
                UTILS_DIE("Not yet implemented!");
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm(
    std::list<genie::core::record::ContactRecord>& recs,
    const EncodingOptions& opt,
    EncodingBlock& block
) {

    auto params = ContactMatrixParameters();
    params.setBinSize(opt.bin_size);
    params.setTileSize(opt.tile_size);
    std::map<uint8_t, SampleInformation> samples;
    std::map<uint8_t, ChromosomeInformation> chrs;

    for (auto& rec: recs){
        auto rec_bin_size = rec.getBinSize();
        UTILS_DIE_IF(rec_bin_size != opt.bin_size, "Found record with different bin_size!");

        if (rec.getChr1ID() > rec.getChr2ID())
            rec.transposeCM();

        uint8_t chr1_ID = rec.getChr1ID();
        uint8_t chr2_ID = rec.getChr2ID();

        auto sample_name = std::string(rec.getSampleName());
        params.addSample(rec.getSampleID(), std::move(sample_name));
        auto chr1_name = std::string(rec.getChr1Name());
        params.upsertChromosome(
            chr1_ID,
            std::move(chr1_name),
            rec.getChr1Length()
        );
        auto chr2_name = std::string(rec.getChr2Name());
        params.upsertChromosome(
            chr2_ID,
            std::move(chr2_name),
            rec.getChr2Length()
        );

        auto scm_payload = genie::contact::SubcontactMatrixPayload(
            0, //TODO(yeremia): change the default parameter_set_ID
            rec.getSampleID(),
            chr1_ID,
            chr2_ID
        );

        encode_scm(
            opt,
            params,
            rec,
            scm_payload
        );
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<ContactMatrixParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::ContactRecord>& recs){}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------
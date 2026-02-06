/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_coder_xtensor.h"

#include <codecs/include/mpegg-codecs.h>

#include <cmath>
#include <fstream>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>

#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::genotype::detail::xtensor {

// -----------------------------------------------------------------------------

void decompose(
    // Input
    std::vector<core::record::VariantGenotype>& recs,
    // Output
    uint8_t& max_ploidy, Int8MatDtype& allele_mat, BinMatDtype& phasing_mat,
    // Options
    size_t block_size) {
  UTILS_DIE_IF(recs.empty(), "No records found for the process!");

  block_size = block_size < recs.size() ? block_size : recs.size();
  uint32_t num_samples = recs.front().GetSampleCount();

  uint32_t i_rec = 0;
  // Precompute max ploidy do avoid resizing during process
  max_ploidy = 0;
  for (i_rec = 0; i_rec < block_size; i_rec++) {
    auto& rec = recs[i_rec];

    UTILS_DIE_IF(num_samples != rec.GetSampleCount(),
                 "Number of samples is not constant within a block!");

    auto tmp_p = static_cast<uint8_t>(rec.GetNumberOfAllelesPerSample());
    max_ploidy = max_ploidy > tmp_p ? max_ploidy : tmp_p;
  }

  auto xt_allele_mat = xt::empty<int8_t, xt::layout_type::row_major>({block_size, num_samples * max_ploidy});
  xt_allele_mat.fill(-2);  // Initialize value with "not available"

  auto xt_phasing_mat = xt::zeros<bool, xt::layout_type::row_major>(
      {block_size, num_samples * static_cast<uint8_t>(max_ploidy > 0 ? max_ploidy - 1 : 0)});

  for (i_rec = 0; i_rec < block_size; i_rec++) {
    auto& rec = recs[i_rec];

    auto& rec_alleles = rec.GetAlleles();
    for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
      for (uint8_t k_allele = 0; k_allele < rec.GetNumberOfAllelesPerSample();
           k_allele++) {
        xt_allele_mat(i_rec, j_sample * max_ploidy + k_allele) =
            static_cast<int8_t>(rec_alleles[j_sample][k_allele]);
      }
    }

    if (max_ploidy > 1) {
      unsigned int phasing_ploidy = max_ploidy - 1;
      auto& rec_phasings = rec.GetPhasing();
      for (uint32_t j_sample = 0; j_sample < num_samples; j_sample++) {
        for (uint8_t k_allele = 0;
             k_allele < rec.GetNumberOfAllelesPerSample() - 1; k_allele++) {
          xt_phasing_mat(i_rec, j_sample * phasing_ploidy + k_allele) =
              rec_phasings[j_sample][k_allele];
        }
      }
    }
  }
  
  // Convert back to std::vector
  allele_mat.assign(block_size, std::vector<int8_t>(num_samples * max_ploidy));
  for (size_t i = 0; i < block_size; ++i) {
      for (size_t j = 0; j < num_samples * max_ploidy; ++j) {
          allele_mat[i][j] = xt_allele_mat(i, j);
      }
  }
  
  size_t phasing_ncols = num_samples * (max_ploidy > 0 ? max_ploidy - 1 : 0);
  phasing_mat.assign(block_size, std::vector<bool>(phasing_ncols));
  for (size_t i = 0; i < block_size; ++i) {
      for (size_t j = 0; j < phasing_ncols; ++j) {
          phasing_mat[i][j] = xt_phasing_mat(i, j);
      }
  }
}

// -----------------------------------------------------------------------------

void transform_max_value(Int8MatDtype& allele_mat, bool& no_ref_flag,
                         bool& not_avail_flag) {
  if (allele_mat.empty()) return;
  auto nrows = allele_mat.size();
  auto ncols = allele_mat[0].size();

  std::vector<int8_t> flat_mat;
  flat_mat.reserve(nrows * ncols);
  for (const auto& row : allele_mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
  auto xt_allele_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

  no_ref_flag = xt::any(xt::equal(xt_allele_mat, -1));
  not_avail_flag = xt::any(xt::equal(xt_allele_mat, -2));

  auto max_val = static_cast<int8_t>(xt::amax(xt_allele_mat)(0));

  if (no_ref_flag) {
    max_val++;
    xt::filter(xt_allele_mat, xt::equal(xt_allele_mat, -1)) = max_val;
  }

  if (not_avail_flag) {
    max_val++;
    xt::filter(xt_allele_mat, xt::equal(xt_allele_mat, -2)) = max_val;
  }
  
  for (size_t i = 0; i < nrows; ++i) {
      for (size_t j = 0; j < ncols; ++j) {
          allele_mat[i][j] = xt_allele_mat(i, j);
      }
  }
}

// -----------------------------------------------------------------------------

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag,
                               bool not_avail_flag) {
  if (allele_mat.empty()) return;
  auto nrows = allele_mat.size();
  auto ncols = allele_mat[0].size();

  std::vector<int8_t> flat_mat;
  flat_mat.reserve(nrows * ncols);
  for (const auto& row : allele_mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
  auto xt_allele_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

  auto max_val = static_cast<signed char>(xt::amax(xt_allele_mat)(0));

  if (not_avail_flag) {
    xt::filter(xt_allele_mat, xt::equal(xt_allele_mat, max_val)) = -2;
    max_val--;
  }

  if (no_ref_flag) {
    xt::filter(xt_allele_mat, xt::equal(xt_allele_mat, max_val)) = -1;
  }
  
  for (size_t i = 0; i < nrows; ++i) {
      for (size_t j = 0; j < ncols; ++j) {
          allele_mat[i][j] = xt_allele_mat(i, j);
      }
  }
}

// -----------------------------------------------------------------------------

void binarize_bit_plane(
    // Input
    Int8MatDtype& allele_mat,
    // Output
    std::vector<BinMatDtype>& bin_mats, uint8_t& num_bit_planes,
    // Options
    const ConcatAxis concat_axis
) {
  if (allele_mat.empty()) return;
  auto nrows = allele_mat.size();
  auto ncols = allele_mat[0].size();

  std::vector<int8_t> flat_mat;
  flat_mat.reserve(nrows * ncols);
  for (const auto& row : allele_mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
  auto xt_allele_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

  auto max_val = static_cast<uint8_t>(xt::amax(xt_allele_mat)(0));
  num_bit_planes = static_cast<uint8_t>(std::ceil(std::log2(max_val + 1)));
  if (num_bit_planes == 0) num_bit_planes = 1;

  std::vector<xt::xtensor<bool, 2>> xt_bin_mats(num_bit_planes);

  for (uint8_t k = 0; k < num_bit_planes; k++) {
    xt_bin_mats[k] = xt::eval(xt::cast<bool>(xt_allele_mat & (1 << k)));
  }

  if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
    for (uint8_t k = 1; k < num_bit_planes; k++) {
      if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
        xt_bin_mats[0] =
            xt::eval(xt::concatenate(xt::xtuple(xt_bin_mats[0], xt_bin_mats[k]), 0));
      } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
        xt_bin_mats[0] =
            xt::eval(xt::concatenate(xt::xtuple(xt_bin_mats[0], xt_bin_mats[k]), 1));
      }
    }
    // Clean-up the remaining bin_mats
    xt_bin_mats.resize(1);
  }

  bin_mats.clear();
  for (const auto& xt_bin_mat : xt_bin_mats) {
      size_t r = xt_bin_mat.shape(0);
      size_t c = xt_bin_mat.shape(1);
      BinMatDtype b(r, std::vector<bool>(c));
      for (size_t i = 0; i < r; ++i) {
          for (size_t j = 0; j < c; ++j) {
              b[i][j] = xt_bin_mat(i, j);
          }
      }
      bin_mats.push_back(std::move(b));
  }

  allele_mat.clear();
}

// -----------------------------------------------------------------------------

void debinarize_bit_plane(
    // Input
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    // Output
    Int8MatDtype& allele_mat
) {
  if (bin_mats.empty()) return;
  
  std::vector<xt::xtensor<bool, 2>> xt_bin_mats;
  for (const auto& b : bin_mats) {
      size_t r = b.size();
      size_t c = b.empty() ? 0 : b[0].size();
      xt::xtensor<bool, 2> xt_b = xt::empty<bool>({r, c});
      for (size_t i = 0; i < r; ++i) {
          for (size_t j = 0; j < c; ++j) {
              xt_b(i, j) = b[i][j];
          }
      }
      xt_bin_mats.push_back(std::move(xt_b));
  }

  auto& bin_mat = xt_bin_mats.front();
  if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
    size_t nrows = bin_mat.shape(0);
    auto new_nrows = nrows / num_bit_planes;
    for (auto k = 0u; k < num_bit_planes; k++) {
      xt::xtensor<bool, 2> curr_bin_mat = xt::view(
          bin_mat, xt::range((k)*new_nrows, (k + 1) * new_nrows), xt::all());
      xt_bin_mats.emplace_back(std::move(curr_bin_mat));
    }
  } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
    size_t ncols = bin_mat.shape(1);
    auto new_ncols = ncols / num_bit_planes;
    for (auto k = 0u; k < num_bit_planes; k++) {
      xt::xtensor<bool, 2> curr_bin_mat = xt::view(
          bin_mat, xt::all(), xt::range((k)*new_ncols, (k + 1) * new_ncols));
      xt_bin_mats.emplace_back(std::move(curr_bin_mat));
    }
  }

  if (concat_axis == ConcatAxis::CONCAT_ROW_DIR ||
      concat_axis == ConcatAxis::CONCAT_COL_DIR) {
    xt_bin_mats.erase(xt_bin_mats.begin());
  }

  auto num_bin_mats = xt_bin_mats.size();
  size_t final_nrows = xt_bin_mats.front().shape(0);
  size_t final_ncols = xt_bin_mats.front().shape(1);
  auto xt_allele_mat = xt::zeros<int8_t>({final_nrows, final_ncols});
  for (auto k = 0u; k < num_bin_mats; k++) {
    xt_allele_mat += xt::cast<int8_t>(xt_bin_mats[k]) << k;
  }
  
  allele_mat.assign(final_nrows, std::vector<int8_t>(final_ncols));
  for (size_t i = 0; i < final_nrows; ++i) {
      for (size_t j = 0; j < final_ncols; ++j) {
          allele_mat[i][j] = xt_allele_mat(i, j);
      }
  }
}

// -----------------------------------------------------------------------------

void binarize_row_bin(
    // Input
    Int8MatDtype& allele_mat,
    // Output
    std::vector<BinMatDtype>& bin_mats,
//    uint8_t& num_bit_planes,
    UIntVecDtype& amax_vec
) {
    if (allele_mat.empty()) return;
    auto nrows = allele_mat.size();
    auto ncols = allele_mat[0].size();

    std::vector<int8_t> flat_mat;
    flat_mat.reserve(nrows * ncols);
    for (const auto& row : allele_mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
    auto xt_allele_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

  auto xt_amax_vec =
      xt::cast<uint8_t>(xt::ceil(xt::log2(xt::amax(xt_allele_mat, {1}) + 1)));
  // Handle the case where maximum value is 0 -> log2(1) = 0 bits
  xt::filter(xt_amax_vec, xt::equal(xt_amax_vec, 0u)) = 1;

  auto bin_mat_nrows = static_cast<size_t>(xt::sum(xt_amax_vec)(0));

  auto xt_bin_mat = xt::zeros<bool>({bin_mat_nrows, ncols});

  size_t i2 = 0;
  for (size_t i = 0; i < nrows; i++) {
    for (size_t i_bit = 0; i_bit < xt_amax_vec[i]; i_bit++) {
      xt::view(xt_bin_mat, i2++, xt::all()) =
          xt::cast<bool>(xt::view(xt_allele_mat, i, xt::all()) & (1 << i_bit));
    }
  }

  BinMatDtype b(bin_mat_nrows, std::vector<bool>(ncols));
  for (size_t i = 0; i < bin_mat_nrows; ++i) {
      for (size_t j = 0; j < ncols; ++j) {
          b[i][j] = xt_bin_mat(i, j);
      }
  }
  bin_mats.emplace_back(std::move(b));
  
  amax_vec.assign(xt_amax_vec.begin(), xt_amax_vec.end());

  allele_mat.clear();
}

// -----------------------------------------------------------------------------

void debinarize_row_bin(
  // Inputs
  std::vector<BinMatDtype>& bin_mats,
  UIntVecDtype& amax_vec,
  // Outputs
  Int8MatDtype& allele_mat
) {
  if (bin_mats.empty()) return;
  auto& bin_mat = bin_mats.front();
  auto nrows = amax_vec.size();
  auto ncols = bin_mat.empty() ? 0 : bin_mat[0].size();

  // Initialize values, has to be zero
  allele_mat.assign(nrows, std::vector<int8_t>(ncols, 0));

  int l = 0;
  for (auto i = 0u; i < nrows; i++) {
    for (auto j = 0u; j < amax_vec[i]; j++) {
      for (size_t col = 0; col < ncols; ++col) {
          if (bin_mat[l][col]) allele_mat[i][col] += (1 << j);
      }
      l++;
    }
  }
}

// -----------------------------------------------------------------------------

void binarize_allele_mat(
    // Input
    Int8MatDtype& allele_mat,
    // Output
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    UIntVecDtype& amax_vec,
    // Options
    BinarizationID binarization_ID,
    ConcatAxis concat_axis
) {

  switch (binarization_ID) {
      case genie::genotype::BinarizationID::BIT_PLANE: {
          detail::xtensor::binarize_bit_plane(
              allele_mat,
              bin_mats,
              num_bit_planes,
              concat_axis
          );
          break;
      }
      case genie::genotype::BinarizationID::ROW_BIN: {
          num_bit_planes = 1;

          detail::xtensor::binarize_row_bin(
              allele_mat,
              bin_mats,
//                num_bit_planes,
              amax_vec
          );
          break;
      }
      default:
          UTILS_DIE("Invalid binarization_ID_!");
  }

  // TODO(Yeremia): Free memory of allele_mat
  allele_mat.clear();
}

// -----------------------------------------------------------------------------

void inverse_binarize_allele_mat(
    // Input
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis,
    UIntVecDtype& amax_vec,
    // Output
    Int8MatDtype& allele_mat
){

}

// -----------------------------------------------------------------------------

void sort_matrix(
  BinMatDtype& bin_mat,
  const UIntVecDtype& ids,
  uint8_t axis
) {
  UTILS_DIE_IF(axis > 1, "Invalid axis value!");
  if (bin_mat.empty()) return;
  size_t nrows = bin_mat.size();
  size_t ncols = bin_mat[0].size();
  UTILS_DIE_IF((axis == 0 ? nrows : ncols) != ids.size(),
               "bin_mat and ids have different dimension!");

  if (ids.empty()) return;

  if (axis == 0) {
    BinMatDtype tmp_bin_mat(nrows);
    for (size_t i = 0; i < nrows; i++) {
      tmp_bin_mat[i] = bin_mat[ids[i]];
    }
    bin_mat = std::move(tmp_bin_mat);
  } else if (axis == 1) {
    BinMatDtype tmp_bin_mat(nrows, std::vector<bool>(ncols));
    for (size_t i = 0; i < nrows; i++) {
      for (size_t j = 0; j < ncols; j++) {
          tmp_bin_mat[i][j] = bin_mat[i][ids[j]];
      }
    }
    bin_mat = std::move(tmp_bin_mat);
  }
}

// -----------------------------------------------------------------------------

void random_sort_bin_mat(BinMatDtype& bin_mat, UIntVecDtype& ids,
                         uint8_t axis) {
  UTILS_DIE_IF(axis > 1, "Invalid axis value!");
  if (bin_mat.empty()) return;
  auto num_elem = static_cast<uint32_t>(axis == 0 ? bin_mat.size() : bin_mat[0].size());
  auto xt_ids = xt::random::permutation<uint32_t>(num_elem);
  ids.assign(xt_ids.begin(), xt_ids.end());
  detail::xtensor::sort_matrix(bin_mat, ids, axis);
  auto xt_argsort_ids = xt::argsort(xt_ids);
  ids.assign(xt_argsort_ids.begin(), xt_argsort_ids.end());
}

// -----------------------------------------------------------------------------

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
) {
    switch (sort_row_method) {
        case genie::genotype::SortingAlgoID::NO_SORTING:
            row_ids = UIntVecDtype({0});
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            detail::xtensor::random_sort_bin_mat(bin_mat, row_ids, 0);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }

    switch (sort_col_method) {
        case genie::genotype::SortingAlgoID::NO_SORTING:
            col_ids = UIntVecDtype({0});
            break;
        case genie::genotype::SortingAlgoID::RANDOM_SORT:
            detail::xtensor::random_sort_bin_mat(bin_mat, col_ids, 1);
            break;
        case genie::genotype::SortingAlgoID::NEAREST_NEIGHBOR:
            UTILS_DIE("Not yet implemented!");
            break;
        case genie::genotype::SortingAlgoID::LIN_KERNIGHAN_HEURISTIC:
            UTILS_DIE("Not yet implemented!");
            break;
        default:
            UTILS_DIE("Invalid sort method!");
    }
}

// -----------------------------------------------------------------------------

void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
) {
  if (row_ids.size() != 1) {
    detail::xtensor::sort_matrix(bin_mat, row_ids, 0);
    row_ids.resize(0);
  }
  if (col_ids.size() != 1) {
    detail::xtensor::sort_matrix(bin_mat, col_ids, 1);
    col_ids.resize(0);
  }
}

// -----------------------------------------------------------------------------

// void sort_block(
//     const EncodingOptions& opt,
//     EncodingBlock& block
//) {
//     auto num_bin_mats = GetNumBinMats(block);
//
//     block.allele_row_ids_vect.resize(num_bin_mats);
//     block.allele_col_ids_vect.resize(num_bin_mats);
//
//     for (uint8_t i_mat = 0; i_mat < num_bin_mats; i_mat++) {
//         sort_bin_mat(block.allele_bin_mat_vect[i_mat],
//         block.allele_row_ids_vect[i_mat],
//                      block.allele_col_ids_vect[i_mat], opt.sort_row_method,
//                      opt.sort_col_method);
//     }
//
//     // Sort phasing matrix
//     sort_bin_mat(
//         block.phasing_mat,
//         block.phasing_row_ids,
//         block.phasing_col_ids,
//         opt.sort_row_method,
//         opt.sort_col_method
//     );
// }

// -----------------------------------------------------------------------------

void bin_mat_to_bytes(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    uint8_t** payload,
    size_t& payload_len
) {
  auto nrows = bin_mat.size();
  if (nrows == 0) {
      payload_len = 0;
      *payload = nullptr;
      return;
  }
  auto ncols = bin_mat[0].size();

  auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // Ceil div operation
  payload_len = bpl * nrows;
  *payload = (unsigned char*)calloc(payload_len, sizeof(unsigned char));

  for (auto i = 0u; i < nrows; i++) {
    size_t row_offset = i * bpl;
    for (auto j = 0u; j < ncols; j++) {
      auto byte_offset = row_offset + (j >> 3u);
      uint8_t shift = (7u - (j & 7u));
      auto bool_val = bin_mat[i][j];
      auto val = static_cast<uint8_t>(bool_val);
      val = static_cast<uint8_t>(val << shift);
      *(*payload + byte_offset) |= val;
    }
  }
}

// -----------------------------------------------------------------------------

void bin_mat_from_bytes(
    // Inputs
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    // Outputs
    BinMatDtype& bin_mat
) {
  auto bpl = (ncols >> 3u) +
             ((ncols & 7u) > 0u);  // bytes per line with ceil operation
  UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl),
               "Invalid payload_len / nrows / ncols!");

  bin_mat.assign(nrows, std::vector<bool>(ncols, false));

  for (auto i = 0u; i < nrows; i++) {
    size_t row_offset = i * bpl;
    for (auto j = 0u; j < ncols; j++) {
      auto byte_offset = row_offset + (j >> 3u);
      uint8_t shift = (7u - (j & 7u));
      bin_mat[i][j] = (*(payload + byte_offset) >> shift) & 1u;
    }
  }
}

// -----------------------------------------------------------------------------
// TODO (Yeremia,Stefanie): Move and refactor this function to the parsing
// funciton

// void sort_format(const std::vector<core::record::VariantGenotype>& recs,
// size_t block_size, EncodingBlock& block) {
//     // starting number
//     uint8_t AttributeID = 25;
//
//     // fill all attribute data
//     for (const auto& format : recs.at(0).GetFormat()) {
//         const auto& formatName = format.getFormat();
//         block.attributeData[formatName].resize(block_size);
//         block.attributeInfo[formatName] =
//         core::record::annotation_parameter_set::AttributeData(
//             static_cast<uint8_t>(formatName.size()), formatName,
//             format.getType(), format.getArrayLength(), AttributeID);
//         AttributeID++;
//     }
//
//     // add values
//     for (auto i_rec = 0u; i_rec < block_size; i_rec++) {
//         auto& rec = recs[i_rec];
//         for (const auto& format : rec.GetFormat()) {
//             auto formatName = format.getFormat();
//             std::vector<std::vector<AttrType>> formatValue =
//             format.getValue(); block.attributeData[formatName].at(i_rec) =
//             formatValue;
//         }
//     }
// }

// -----------------------------------------------------------------------------

void entropy_encode_bin_mat(
    // Inputs
    const BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    // Outputs
    std::vector<uint8_t>& payload
) {
    uint8_t* raw_data;
    size_t raw_data_len;
    uint8_t* compressed_data;
    size_t compressed_data_len;

    detail::xtensor::bin_mat_to_bytes(bin_mat, &raw_data, raw_data_len);

    switch (codec_ID) {
      case genie::core::AlgoID::JBIG: {
        auto nrows = static_cast<unsigned long>(bin_mat.size());
        auto ncols = static_cast<unsigned long>(bin_mat.empty() ? 0 : bin_mat[0].size());
        mpegg_jbig_compress_default(
            &compressed_data,
            &compressed_data_len,
            raw_data,
            raw_data_len,
            nrows,
            ncols
        );
      } break;
      case genie::core::AlgoID::ZSTD: {
        mpegg_zstd_compress(
            &compressed_data,
            &compressed_data_len,
            raw_data,
            raw_data_len,
            3
        );
      } break;
      case genie::core::AlgoID::BSC: {
        mpegg_bsc_compress(
            &compressed_data,
            &compressed_data_len,
            raw_data,
            raw_data_len,
            20, // lzpHashSize
            8,  // lzpMinLen
            1, // LIBBSC_BLOCKSORTER_BWT
            1
        );
      } break;
      case genie::core::AlgoID::LZMA: {
        UTILS_DIE("Not yet implemented!");
      } break;
      default:
        UTILS_DIE("Invalid codec_ID");
        break;
    }

    payload = std::vector<uint8_t>(compressed_data, compressed_data + compressed_data_len);

    free(raw_data);
}

void entropy_decode_bin_mat(
    const std::vector<uint8_t>& payload,
    genie::core::AlgoID codec_ID,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
){
  uint8_t* raw_data = nullptr;
  size_t raw_data_len = 0;

  switch (codec_ID) {
    case genie::core::AlgoID::JBIG: {
      mpegg_jbig_decompress_default(
          &raw_data,
          &raw_data_len,
          payload.data(),
          payload.size(),
          reinterpret_cast<unsigned long*>(&nrows), 
          reinterpret_cast<unsigned long*>(&ncols)
      );
    } break;
    case genie::core::AlgoID::ZSTD: {
      mpegg_zstd_decompress(
          &raw_data,
          &raw_data_len,
          payload.data(),
          payload.size()
      );
    } break;
    case genie::core::AlgoID::BSC: {
      mpegg_bsc_decompress(
          &raw_data,
          &raw_data_len,
          payload.data(),
          payload.size()
      );
    } break;
    case genie::core::AlgoID::LZMA: {
      UTILS_DIE("Not yet implemented!");
    } break;
    default:
      UTILS_DIE("Invalid codec_ID");
      break;
  }

  bin_mat_from_bytes(
    raw_data,
    raw_data_len,
    nrows,
    ncols,
    bin_mat
  );
  if (raw_data) free(raw_data);
}

// -----------------------------------------------------------------------------

void encode_and_sort_bin_mat(
    // Inputs
    BinMatDtype& bin_mat,
    // Output
    SortedBinMatPayload& sorted_bin_mat_payload,
    // Options
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
){

  auto sort_rows_flag = sort_row_method != SortingAlgoID::NO_SORTING;
  auto sort_cols_flag = sort_col_method != SortingAlgoID::NO_SORTING;

  UIntVecDtype row_ids;
  UIntVecDtype col_ids;

  detail::xtensor::sort_bin_mat(
      bin_mat,
      row_ids,
      col_ids,
      sort_row_method,
      sort_col_method
  );

  if (sort_rows_flag) {
    RowColIdsPayload row_ids_payload(row_ids);

    sorted_bin_mat_payload.SetRowIdsPayload(
        std::move(row_ids_payload)
    );
  }

  if (sort_cols_flag) {
    RowColIdsPayload col_ids_payload(col_ids);

    sorted_bin_mat_payload.SetColIdsPayload(
        std::move(col_ids_payload)
    );
  }

  // Handle entropy coding
  {
    auto nrows = static_cast<uint32_t>(bin_mat.size());
    auto ncols = static_cast<uint32_t>(bin_mat.empty() ? 0 : bin_mat[0].size());
    std::vector<uint8_t> payload;

    detail::xtensor::entropy_encode_bin_mat(
        bin_mat,
        codec_ID,
        payload
    );

    BinMatPayload bin_mat_payload(
        codec_ID,
        std::move(payload),
        nrows,
        ncols
    );

    sorted_bin_mat_payload.SetBinMatPayload(
        std::move(bin_mat_payload)
    );
  }
}

// -----------------------------------------------------------------------------

void decode_and_inverse_sort_bin_mat(
  // Inputs
  const SortedBinMatPayload& sorted_bin_mat_payload,
  // Output
  BinMatDtype& bin_mat,
  // Options
  genie::core::AlgoID codec_ID,
  bool sort_rows_flag,
  bool sort_cols_flag
){

  auto& bin_mat_payload = sorted_bin_mat_payload.GetBinMatPayload();

  detail::xtensor::entropy_decode_bin_mat(
    bin_mat_payload.GetPayload(),
    codec_ID,
    static_cast<size_t>(bin_mat_payload.GetNRows()),
    static_cast<size_t>(bin_mat_payload.GetNCols()),
    bin_mat
  );

  if (sort_rows_flag) {
    auto row_ids = sorted_bin_mat_payload.GetRowIdsPayload()->GetRowColIdsElements();
    detail::xtensor::sort_matrix(bin_mat, row_ids, 0);
  }

  if (sort_cols_flag) {
    auto col_ids = sorted_bin_mat_payload.GetColIdsPayload()->GetRowColIdsElements();
    detail::xtensor::sort_matrix(bin_mat, col_ids, 1);
  }
}

// -----------------------------------------------------------------------------

void encode_genotype(
    // Inputs
    std::vector<core::record::VariantGenotype>& recs,
    // Outputs
    GenotypeParameters& params,
    GenotypePayload& payload,
    // Options
    size_t block_size,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis,
    bool transpose_mat,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
) {
    UTILS_DIE_IF(binarization_ID == BinarizationID::UNDEFINED, "Invalid BinarizationID");
    UTILS_DIE_IF(sort_row_method == SortingAlgoID::UNDEFINED, "Invalid SortingAlgoID");
    UTILS_DIE_IF(sort_col_method == SortingAlgoID::UNDEFINED, "Invalid SortingAlgoID");

    auto tmp_payload = GenotypePayload();

    auto tmp_params = GenotypeParameters(
        binarization_ID,
        concat_axis,
        sort_row_method != SortingAlgoID::NO_SORTING,
        sort_col_method != SortingAlgoID::NO_SORTING,
        transpose_mat,
        codec_ID,
        true, // Encode Phase data
        sort_row_method != SortingAlgoID::NO_SORTING,
        sort_col_method != SortingAlgoID::NO_SORTING,
        transpose_mat,
        codec_ID
    );

    uint8_t max_ploidy;
    bool dot_flag;
    bool na_flag;

    std::vector<BinMatDtype> allele_bin_mat_vect;

    BinMatDtype phasing_mat;
    uint8_t num_bit_planes;

    {
        Int8MatDtype allele_mat;
        detail::xtensor::decompose(
            recs,
            max_ploidy,
            allele_mat,
            phasing_mat,
            block_size
        );

        tmp_payload.SetMaxPloidy(max_ploidy);

        detail::xtensor::transform_max_value(
            allele_mat,
            dot_flag,
            na_flag
        );

        tmp_payload.SetNoReferenceFlag(dot_flag);
        tmp_payload.SetNotAvailableFlag(na_flag);

        UIntVecDtype amax_vec;

        detail::xtensor::binarize_allele_mat(
            allele_mat,
            allele_bin_mat_vect,
            num_bit_planes,
            amax_vec,
            tmp_params.GetBinarizationID(),
            tmp_params.GetConcatAxis()
        );

        tmp_payload.SetNumBitPlanes(num_bit_planes);

        if (tmp_params.GetBinarizationID() == BinarizationID::ROW_BIN){
            std::vector<uint64_t> amax_elements(amax_vec.begin(), amax_vec.end());
            AmaxPayload amax_payload(std::move(amax_elements));
            tmp_payload.SetVariantsAmaxPayload(std::move(amax_payload));
        }
    }

    auto num_variants_payloads = 1;
    // Special case for num_variants_payloads
    if (tmp_params.GetBinarizationID() == BinarizationID::BIT_PLANE && tmp_params.GetConcatAxis() == ConcatAxis::DO_NOT_CONCAT){
      num_variants_payloads = num_bit_planes;
    }

    for (auto i_mat = 0u; i_mat < (uint32_t)num_variants_payloads; i_mat++) {
      genie::genotype::SortedBinMatPayload sorted_bin_mat_payload;

      detail::xtensor::encode_and_sort_bin_mat(
        // Inputs
        allele_bin_mat_vect[i_mat],
        // Outputs
        sorted_bin_mat_payload,
        // Options
        sort_row_method,
        sort_col_method,
        codec_ID
      );

      tmp_payload.AddVariantsPayload(
        std::move(sorted_bin_mat_payload)
      );
    }

    if (tmp_params.GetEncodePhasesDataFlag()) {
      genie::genotype::SortedBinMatPayload sorted_bin_mat_payload;

      detail::xtensor::encode_and_sort_bin_mat(
        phasing_mat,
        sorted_bin_mat_payload,
        sort_row_method,
        sort_col_method,
        codec_ID
      );

      tmp_payload.SetPhasesPayload(
          std::move(sorted_bin_mat_payload)
      );
    }

    params = std::move(tmp_params);
    payload = std::move(tmp_payload);
}

// -----------------------------------------------------------------------------

void decode_genotype(
    const GenotypeParameters& params,
    const GenotypePayload& payload,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat
) {
  std::vector<BinMatDtype> bin_mats;

  for (const auto& sorted_bin_mat_payload : payload.GetVariantsPayloads()){
    BinMatDtype recon_bin_mat;
    detail::xtensor::decode_and_inverse_sort_bin_mat(
        sorted_bin_mat_payload,
        recon_bin_mat,
        params.GetVariantsCodecID(),
        params.GetSortVariantsRowsFlag(),
        params.GetSortVariantsColsFlag()
    );

    bin_mats.emplace_back(std::move(recon_bin_mat));
  }

  if (params.GetBinarizationID() == BinarizationID::ROW_BIN){
      auto amax_elements = payload.GetVariantsAmaxPayload()->GetAmaxElements();
      UIntVecDtype amax_vec(amax_elements.begin(), amax_elements.end());
      detail::xtensor::debinarize_row_bin(
          bin_mats,
          amax_vec,
          allele_mat
      );
  } else if (params.GetBinarizationID() == BinarizationID::BIT_PLANE) {
      detail::xtensor::debinarize_bit_plane(
          bin_mats,
          payload.GetNumBitPlanes(),
          params.GetConcatAxis(),
          allele_mat
      );
  }

  if (params.GetEncodePhasesDataFlag()) {
      const auto& phasing_payload = payload.GetPhasesPayload();
      BinMatDtype decoded_phasing_mat;

      detail::xtensor::decode_and_inverse_sort_bin_mat(
          *phasing_payload,
          decoded_phasing_mat,
          params.GetPhasesCodecID(),
          params.GetSortPhasesRowsFlag(),
          params.GetSortPhasesColsFlag()
      );
      phasing_mat = decoded_phasing_mat;
  }

  detail::xtensor::inverse_transform_max_val(
      allele_mat,
      payload.GetNoReferenceFlag(),
      payload.GetNotAvailableFlag()
  );
}

// -----------------------------------------------------------------------------

}  // namespace genie::genotype

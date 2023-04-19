/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "GenotypeParameters.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

GenotypeParameters::GenotypeParameters()
    : max_ploidy(0),
      no_reference_flag(true),
      not_available_flag(true),
      binarization_ID(BinarizationID::BIT_PLANE),
      num_bit_plane(0),
      concat_axis(ConcatAxis::DO_NOT_CONCAT),
      sort_variants_rows_flag{},
      sort_variants_cols_flag{},
      transpose_variants_mat_flag{},
      variants_codec_ID{},
      encode_phases_data_flag(false),
      sort_phases_rows_flag(false),
      sort_phases_cols_flag(false),
      transpose_phases_mat_flag(false),
      phases_codec_ID(false),
      phases_value(false) {
    num_variants_payloads = 1;
    if (concat_axis == ConcatAxis::DO_NOT_CONCAT && binarization_ID == BinarizationID::BIT_PLANE)
        num_variants_payloads = num_bit_plane;
}

GenotypeParameters::GenotypeParameters(util::BitReader& reader) { read(reader); }

GenotypeParameters::GenotypeParameters(uint8_t max_ploidy, bool no_reference_flag, bool not_available_flag,
                                       annotation_encoding_parameters::BinarizationID binarization_ID,
                                       uint8_t num_bit_plane, ConcatAxis concat_axis,
                                       std::vector<bool> sort_variants_rows_flag,
                                       std::vector<bool> sort_variants_cols_flag,
                                       std::vector<bool> transpose_variants_mat_flag,
                                       std::vector<uint8_t> variants_codec_ID, bool encode_phases_data_flag,
                                       bool sort_phases_rows_flag, bool sort_phases_cols_flag,
                                       bool transpose_phases_mat_flag, bool phases_codec_ID, bool phases_value)
    : max_ploidy(max_ploidy),
      no_reference_flag(no_reference_flag),
      not_available_flag(not_available_flag),
      binarization_ID(binarization_ID),
      num_bit_plane(num_bit_plane),
      concat_axis(concat_axis),
      sort_variants_rows_flag(sort_variants_rows_flag),
      sort_variants_cols_flag(sort_variants_cols_flag),
      transpose_variants_mat_flag(transpose_variants_mat_flag),
      variants_codec_ID(variants_codec_ID),
      encode_phases_data_flag(encode_phases_data_flag),
      sort_phases_rows_flag(sort_phases_rows_flag),
      sort_phases_cols_flag(sort_phases_cols_flag),
      transpose_phases_mat_flag(transpose_phases_mat_flag),
      phases_codec_ID(phases_codec_ID),
      phases_value(phases_value) {
    num_variants_payloads = 1;
    if (concat_axis == ConcatAxis::DO_NOT_CONCAT && binarization_ID == BinarizationID::BIT_PLANE)
        num_variants_payloads = num_bit_plane;
}

void GenotypeParameters::read(util::BitReader& reader) {
    sort_variants_rows_flag.resize(0);
    sort_variants_cols_flag.resize(0);
    transpose_variants_mat_flag.resize(0);
    variants_codec_ID.resize(0);

    max_ploidy = static_cast<uint8_t>(reader.read_b(8));
    no_reference_flag = static_cast<bool>(reader.read_b(1));
    not_available_flag = static_cast<bool>(reader.read_b(1));
    binarization_ID = static_cast<BinarizationID>(reader.read_b(3));
    if (binarization_ID == BinarizationID::BIT_PLANE) {
        num_bit_plane = static_cast<uint8_t>(reader.read_b(8));
        concat_axis = static_cast<ConcatAxis>(static_cast<uint8_t>(reader.read_b(8)));
    }
    num_variants_payloads = 1;
    if (concat_axis == ConcatAxis::DO_NOT_CONCAT && binarization_ID == BinarizationID::BIT_PLANE)
        num_variants_payloads = num_bit_plane;

    for (auto i = 0; i < num_variants_payloads; ++i) {
        sort_variants_rows_flag.push_back(static_cast<bool>(reader.read_b(1)));
        sort_variants_cols_flag.push_back(static_cast<bool>(reader.read_b(1)));
        transpose_variants_mat_flag.push_back(static_cast<bool>(reader.read_b(1)));
        variants_codec_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
    }
    encode_phases_data_flag = static_cast<bool>(reader.read_b(1));
    if (encode_phases_data_flag) {
        sort_phases_rows_flag = static_cast<bool>(reader.read_b(1));
        sort_phases_cols_flag = static_cast<bool>(reader.read_b(1));
        transpose_phases_mat_flag = static_cast<bool>(reader.read_b(1));
        phases_codec_ID = static_cast<bool>(reader.read_b(1));
    }
    phases_value = static_cast<bool>(reader.read_b(1));
}

void GenotypeParameters::write(util::BitWriter& writer) const {
    writer.write(max_ploidy, 8);
    writer.write(no_reference_flag, 1);
    writer.write(not_available_flag, 1);
    writer.write(static_cast<uint8_t>(binarization_ID), 3);
    if (binarization_ID == BinarizationID::BIT_PLANE) {
        writer.write(num_bit_plane, 8);
        writer.write(static_cast<uint8_t>(concat_axis), 8);
    }

    for (auto i = 0; i < num_variants_payloads; ++i) {
        writer.write(sort_variants_rows_flag[i], 1);
        writer.write(sort_variants_cols_flag[i], 1);
        writer.write(transpose_variants_mat_flag[i], 1);
        writer.write(variants_codec_ID[i], 8);
    }
    writer.write(encode_phases_data_flag, 1);
    if (encode_phases_data_flag) {
        writer.write(sort_phases_rows_flag, 1);
        writer.write(sort_phases_cols_flag, 1);
        writer.write(transpose_phases_mat_flag, 1);
        writer.write(phases_codec_ID, 1);
    } else
        writer.write(phases_value, 1);
}
void GenotypeParameters::write(std::ostream& outputfile) const {
    outputfile << std::to_string(max_ploidy) << ",";
    outputfile << std::to_string(no_reference_flag) << ",";
    outputfile << std::to_string(not_available_flag) << ",";
    outputfile << std::to_string(static_cast<uint8_t>(binarization_ID)) << ",";
    if (binarization_ID == BinarizationID::BIT_PLANE) {
        outputfile << std::to_string(num_bit_plane) << ",";
        outputfile << std::to_string(static_cast<uint8_t>(concat_axis)) << ",";
    }
    for (auto i = 0; i < num_variants_payloads; ++i) {
        outputfile << std::to_string(sort_variants_rows_flag[i]) << ",";
        outputfile << std::to_string(sort_variants_cols_flag[i]) << ",";
        outputfile << std::to_string(transpose_variants_mat_flag[i]) << ",";
        outputfile << std::to_string(variants_codec_ID[i]) << ",";
    }
    outputfile << std::to_string(encode_phases_data_flag) << ",";
    if (encode_phases_data_flag) {
        outputfile << std::to_string(sort_phases_rows_flag) << ",";
        outputfile << std::to_string(sort_phases_cols_flag) << ",";
        outputfile << std::to_string(transpose_phases_mat_flag) << ",";
        outputfile << std::to_string(phases_codec_ID) << ",";
    } else
        outputfile << std::to_string(phases_value);
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

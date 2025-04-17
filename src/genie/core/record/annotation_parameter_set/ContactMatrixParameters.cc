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
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "ContactMatrixParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

ContactMatrixParameters::ContactMatrixParameters()
    : num_samples(0),
      sample_ID{},
      sample_name_len{},
      sample_name{},
      num_chrs(0),
      chr_ID{},
      chr_name_len{},
      chr_name{},
      chr_length{},
      bin_size(0),
      tile_size(0),
      num_bin_size_multipliers(0),
      bin_size_multiplier{},
      num_norm_methods(0),
      norm_method_ID{},
      norm_method_name_len{},
      norm_method_name{},
      norm_method_mult_flag{},
      num_norm_matrices(0),
      norm_matrix_ID{},
      norm_matrix_name_len{},
      norm_matrix_name{} {}

ContactMatrixParameters::ContactMatrixParameters(
    uint8_t num_samples, std::vector<uint8_t> sample_ID, std::vector<uint8_t> sample_name_len,
    std::vector<std::string> sample_name, uint8_t num_chrs, std::vector<uint8_t> chr_ID,
    std::vector<uint8_t> chr_name_len, std::vector<std::string> chr_name, std::vector<uint64_t> chr_length,
    uint32_t bin_size, uint32_t tile_size, uint8_t num_bin_size_multipliers, std::vector<uint32_t> bin_size_multiplier,
    uint8_t num_norm_methods, std::vector<uint8_t> norm_method_ID, std::vector<uint8_t> norm_method_name_len,
    std::vector<std::string> norm_method_name, std::vector<bool> norm_method_mult_flag,
    uint8_t num_norm_matrices, std::vector<uint8_t> norm_matrix_ID, std::vector<uint8_t> norm_matrix_name_len,
    std::vector<std::string> norm_matrix_name)
    : num_samples(num_samples),
      sample_ID(sample_ID),
      sample_name(sample_name),
      num_chrs(num_chrs),
      chr_ID(chr_ID),
      chr_name_len(chr_name_len),
      chr_name(chr_name),
      chr_length(chr_length),
      bin_size(bin_size),
      tile_size(tile_size),
      num_bin_size_multipliers(num_bin_size_multipliers),
      bin_size_multiplier(bin_size_multiplier),
      num_norm_methods(num_norm_methods),
      norm_method_ID(norm_method_ID),
      norm_method_name_len(norm_method_name_len),
      norm_method_name(norm_method_name),
      norm_method_mult_flag(norm_method_mult_flag),
      num_norm_matrices(num_norm_matrices),
      norm_matrix_ID(norm_matrix_ID),
      norm_matrix_name_len(norm_matrix_name_len),
      norm_matrix_name(norm_matrix_name) {}

void ContactMatrixParameters::read(util::BitReader& reader) {
    num_samples = static_cast<uint8_t>(reader.ReadBits(8));

    for (auto i = 0; i < num_samples; ++i) {
        sample_ID.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        sample_name_len.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        std::string name(sample_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<char>(reader.ReadBits(8));
        sample_name.push_back(name);
    }

    num_chrs = static_cast<uint8_t>(reader.ReadBits(8));
    for (auto i = 0; i < num_chrs; ++i) {
        chr_ID.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        chr_name_len.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        std::string name(chr_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<char>(reader.ReadBits(8));
        chr_name.push_back(name);
        chr_length.push_back(reader.ReadBits(64));
    }

    bin_size = static_cast<uint32_t>(reader.ReadBits(32));
    tile_size = static_cast<uint32_t>(reader.ReadBits(32));

    num_bin_size_multipliers = static_cast<uint8_t>(reader.ReadBits(8));
    for (auto i = 0; i < num_bin_size_multipliers; ++i)
        bin_size_multiplier.push_back(static_cast<uint32_t>(reader.ReadBits(32)));

    num_norm_methods = static_cast<uint8_t>(reader.ReadBits(8));
    for (auto i = 0; i < num_norm_methods; ++i) {
        norm_method_ID.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        norm_method_name_len.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        std::string name(norm_method_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<char>(reader.ReadBits(8));
        norm_method_name.push_back(name);
        norm_method_mult_flag.push_back(static_cast<bool>(reader.ReadBits(1)));
    }

    num_norm_matrices = static_cast<uint8_t>(reader.ReadBits(8));
    for (auto i = 0; i < num_norm_matrices; ++i) {
        norm_matrix_ID.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        norm_matrix_name_len.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
        std::string name(norm_matrix_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<char>(reader.ReadBits(8));
        norm_matrix_name.push_back(name);
    }
}

void ContactMatrixParameters::write(core::Writer& writer) const {
    writer.Write(num_samples, 8);
    for (auto i = 0; i < num_samples; ++i) {
        writer.Write(sample_ID[i], 8);
        writer.Write(sample_name_len[i], 8);
        for (auto byte : sample_name[i]) writer.Write(byte, 8);
    }
    writer.Write(num_chrs, 8);
    for (auto i = 0; i < num_chrs; ++i) {
        writer.Write(chr_ID[i], 8);
        writer.Write(chr_name_len[i], 8);
        for (auto byte : chr_name[i]) writer.Write(byte, 8);
        writer.Write(chr_length[i], 64);
    }
    writer.Write(bin_size, 32);
    writer.Write(tile_size, 32);
    writer.Write(num_bin_size_multipliers, 8);
    for (auto i = 0; i < num_bin_size_multipliers; ++i)
      writer.Write(bin_size_multiplier[i], 32);

    writer.Write(num_norm_methods, 8);
    for (auto i = 0; i < num_norm_methods; ++i) {
        writer.Write(norm_method_ID[i], 8);
        writer.Write(norm_method_name_len[i], 8);
        for (auto byte : norm_method_name[i]) writer.Write(byte, 8);
        writer.WriteReserved(7);
        writer.Write(norm_method_mult_flag[i], 1);
    }
    writer.Write(num_norm_matrices, 8);
    for (auto i = 0; i < num_norm_matrices; ++i) {
        writer.Write(norm_matrix_ID[i], 8);
        writer.Write(norm_matrix_name_len[i], 8);
        for (auto byte : norm_matrix_name[i]) writer.Write(byte, 8);
    }
}

size_t ContactMatrixParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.GetBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

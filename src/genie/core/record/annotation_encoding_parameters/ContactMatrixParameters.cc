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

#include "ContactMatrixParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

ContactMatrixParameters::ContactMatrixParameters()
    : num_samples(0),
      sample_ID{},
      sample_name{},
      num_chrs(0),
      chr_ID{},
      chr_name{},
      chr_length{},
      interval(0),
      tile_size(0),
      num_interval_multipliers(0),
      interval_multiplier{},
      num_norm_methods(0),
      norm_method_ID{},
      norm_method_name{},
      norm_method_mult_flag{},
      num_norm_matrices(0),
      norm_matrix_ID{},
      norm_matrix_name{} {}

ContactMatrixParameters::ContactMatrixParameters(
    uint8_t num_samples, std::vector<uint8_t> sample_ID, std::vector<std::string> sample_name, uint8_t num_chrs,
    std::vector<uint8_t> chr_ID, std::vector<std::string> chr_name, std::vector<uint64_t> chr_length, uint32_t interval,
    uint32_t tile_size, uint8_t num_interval_multipliers, std::vector<uint32_t> interval_multiplier,
    uint8_t num_norm_methods, std::vector<uint8_t> norm_method_ID, std::vector<std::string> norm_method_name,
    std::vector<bool> norm_method_mult_flag, uint8_t num_norm_matrices, std::vector<uint8_t> norm_matrix_ID,
    std::vector<std::string> norm_matrix_name)
    : num_samples(num_samples),
      sample_ID(sample_ID),
      sample_name(sample_name),
      num_chrs(num_chrs),
      chr_ID(chr_ID),
      chr_name(chr_name),
      chr_length(chr_length),
      interval(interval),
      tile_size(tile_size),
      num_interval_multipliers(num_interval_multipliers),
      interval_multiplier(interval_multiplier),
      num_norm_methods(num_norm_methods),
      norm_method_ID(norm_method_ID),
      norm_method_name(norm_method_name),
      norm_method_mult_flag(norm_method_mult_flag),
      num_norm_matrices(num_norm_matrices),
      norm_matrix_ID(norm_matrix_ID),
      norm_matrix_name(norm_matrix_name) {}

void ContactMatrixParameters::read(util::BitReader& reader) {
    num_samples = static_cast<uint8_t>(reader.read_b(8));

    for (auto i = 0; i < num_samples; ++i) {
        sample_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        char readchar = 0;
        do {
            readchar = static_cast<char>(reader.read_b(8));
            if (readchar != 0) name += readchar;
        } while (readchar != 0);
        sample_name.push_back(name);
    }

    num_chrs = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_chrs; ++i) {
        chr_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        char readchar = 0;
        do {
            readchar = static_cast<char>(reader.read_b(8));
            if (readchar != 0) name += readchar;
        } while (readchar != 0);
        chr_name.push_back(name);
        chr_length.push_back(reader.read_b(64));
    }

    interval = static_cast<uint32_t>(reader.read_b(32));
    tile_size = static_cast<uint32_t>(reader.read_b(32));

    num_interval_multipliers = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_interval_multipliers; ++i)
        interval_multiplier.push_back(static_cast<uint32_t>(reader.read_b(32)));

    num_norm_methods = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_norm_methods; ++i) {
        norm_method_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        char readchar = 0;
        do {
            readchar = static_cast<char>(reader.read_b(8));
            if (readchar != 0) name += readchar;
        } while (readchar != 0);
        norm_method_name.push_back(name);
        norm_method_mult_flag.push_back(static_cast<bool>(reader.read_b(1)));
    }

    num_norm_matrices = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_norm_matrices; ++i) {
        norm_matrix_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        char readchar = 0;
        do {
            readchar = static_cast<char>(reader.read_b(8));
            if (readchar != 0) name += readchar;
        } while (readchar != 0);
        norm_matrix_name.push_back(name);
    }
}

void ContactMatrixParameters::write(std::ostream& outputfile) const {
    outputfile << std::to_string(num_samples) << ",";
    for (auto i = 0; i < num_samples; ++i) {
        outputfile << std::to_string(sample_ID[i]) << ",";
        outputfile << '"' << sample_name[i] << '"' << ",";
    }
    outputfile << std::to_string(num_chrs) << ",";
    for (auto i = 0; i < num_chrs; ++i) {
        outputfile << std::to_string(chr_ID[i]) << ",";
        outputfile << '"' << chr_name[i] << '"' << ",";
        outputfile << std::to_string(chr_length[i]) << ",";
    }
    outputfile << std::to_string(interval) << ",";
    outputfile << std::to_string(tile_size) << ",";
    outputfile << std::to_string(num_interval_multipliers) << ",";
    for (auto i = 0; i < num_interval_multipliers; ++i) outputfile << std::to_string(interval_multiplier[i]) << ",";
    outputfile << std::to_string(num_norm_methods) << ",";
    for (auto i = 0; i < num_norm_methods; ++i) {
        outputfile << std::to_string(norm_method_ID[i]) << ",";
        outputfile << '"' << norm_method_name[i] << '"' << ",";
        outputfile << std::to_string(norm_method_mult_flag[i]) << ",";
    }
    outputfile << std::to_string(num_norm_matrices) << ",";
    for (auto i = 0; i < num_norm_matrices; ++i){
        outputfile << std::to_string(norm_matrix_ID[i]) << ",";
        outputfile << '"' << norm_matrix_name[i] << '"' << ",";
    }
}

void ContactMatrixParameters::write(util::BitWriter& writer) const {
    writer.write(num_samples, 8);
    for (auto i = 0; i < num_samples; ++i) {
        writer.write(sample_ID[i], 8);
        for (auto byte : sample_name[i]) {
            writer.write(byte, 8);
        }
        writer.write(0, 8);
    }
    writer.write(num_chrs, 8);
    for (auto i = 0; i < num_chrs; ++i) {
        writer.write(chr_ID[i], 8);
        for (auto byte : chr_name[i]) {
            writer.write(byte, 8);
        }
        writer.write(0, 8);
        writer.write(chr_length[i], 64);
    }
    writer.write(interval, 32);
    writer.write(tile_size, 32);
    writer.write(num_interval_multipliers, 8);
    for (auto i = 0; i < num_interval_multipliers; ++i) writer.write(interval_multiplier[i], 32);

    writer.write(num_norm_methods, 8);
    for (auto i = 0; i < num_norm_methods; ++i) {
        writer.write(norm_method_ID[i], 8);
        for (auto byte : norm_method_name[i]) writer.write(byte, 8);
        writer.write(0, 8);
        writer.write(norm_method_mult_flag[i], 1);
        writer.write(0, 7);
    }
    writer.write(num_norm_matrices, 8);
    for (auto i = 0; i < num_norm_matrices; ++i) {
        writer.write(norm_matrix_ID[i], 8);
        for (auto byte : norm_matrix_name[i]) writer.write(byte, 8);
        writer.write(0, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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

#include "AlgorithmParameters.h"
#include "genie/core/record/variant_genotype/arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {
AlgorithmParameters::AlgorithmParameters()
    : n_pars(0), par_ID{}, par_type{}, par_num_array_dims{}, par_array_dims{}, par_val{} {}

AlgorithmParameters::AlgorithmParameters(util::BitReader& reader) { read(reader); }

AlgorithmParameters::AlgorithmParameters(
    uint8_t n_pars, std::vector<uint8_t> par_ID, std::vector<uint8_t> par_type, std::vector<uint8_t> par_num_array_dims,
    std::vector<std::vector<uint8_t>> par_array_dims,
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val)
    : n_pars(n_pars),
      par_ID(par_ID),
      par_type(par_type),
      par_num_array_dims(par_num_array_dims),
      par_array_dims(par_array_dims),
      par_val(par_val) {}

void AlgorithmParameters::read(util::BitReader& reader) {
    n_pars = static_cast<uint8_t>(reader.read_b(4));
    par_array_dims.resize(n_pars);
    par_val.resize(n_pars);

    variant_genotype::arrayType types;

    for (auto i = 0; i < n_pars; ++i) {
        par_ID.push_back(static_cast<uint8_t>(reader.read_b(4)));
        par_type.push_back(static_cast<uint8_t>(reader.read_b(8)));
        par_num_array_dims.push_back(static_cast<uint8_t>(reader.read_b(2)));
        for (auto j = 0; j < par_num_array_dims.back(); ++j) {
            par_array_dims[i].push_back(static_cast<uint8_t>(reader.read_b(8)));
        }
        if (par_num_array_dims.back() == 0)
            par_val[i].resize(1,
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims.back() == 1)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims.back() == 2)
            par_val[i].resize(par_array_dims[i][0], std::vector<std::vector<std::vector<uint8_t>>>(
                                                        par_array_dims[i][1], std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims.back() == 3)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(
                                  par_array_dims[i][1], std::vector<std::vector<uint8_t>>(par_array_dims[i][2])));
        for (auto& d1 : par_val)
            for (auto& d2 : d1)
                for (auto& d3 : d2)
                    for (auto& d4 : d3) d4 = types.toArray(par_type[i], reader);
    }
}

void AlgorithmParameters::write(std::ostream& outputfile) const {
    variant_genotype::arrayType types;
    outputfile << std::to_string(n_pars) << ",";
    for (auto i = 0; i < n_pars; ++i) {
        outputfile << std::to_string(par_ID[i]) << ",";
        outputfile << std::to_string(par_type[i]) << ",";
        outputfile << std::to_string(par_num_array_dims[i]) << ",";
        for (auto j = 0; j < par_num_array_dims[i]; ++j) outputfile << std::to_string(par_array_dims[i][j]) << ",";
        for (auto par_d1 : par_val[i])
            for (auto par_d2 : par_d1)
                for (auto par_d3 : par_d2) outputfile << types.toString(par_type[i], par_d3) << ",";
    }
}

void AlgorithmParameters::write(util::BitWriter& writer) const {
    writer.write(n_pars, 4);
    for (auto i = 0; i < n_pars; ++i) {
        writer.write(par_ID[i], 4);
        writer.write(par_type[i], 8);
        writer.write(par_num_array_dims[i], 2);
        for (auto j = 0; j < par_num_array_dims[i]; ++j) writer.write(par_array_dims[i][j], 8);
        for (auto par_d1 : par_val[i])
            for (auto par_d2 : par_d1)
                for (auto par_d3 : par_d2)
                    for (auto byte : par_d3) writer.write(byte, 8);
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

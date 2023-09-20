/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {
AlgorithmParameters::AlgorithmParameters()
    : n_pars(0), par_ID{}, par_type{}, par_num_array_dims{}, par_array_dims{}, par_val{} {}

AlgorithmParameters::AlgorithmParameters(util::BitReader& reader) { read(reader); }

AlgorithmParameters::AlgorithmParameters(
    uint8_t n_pars, std::vector<uint8_t> par_ID, std::vector<core::DataType> par_type,
    std::vector<uint8_t> par_num_array_dims, std::vector<std::vector<uint8_t>> par_array_dims,
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
    par_ID.resize(n_pars);
    par_type.resize(n_pars);
    par_num_array_dims.resize(n_pars);

    ArrayType types;

    for (auto i = 0; i < n_pars; ++i) {
        par_ID[i] = (static_cast<uint8_t>(reader.read_b(4)));
        par_type[i] = (static_cast<core::DataType>(reader.read_b(8)));
        par_num_array_dims[i] = (static_cast<uint8_t>(reader.read_b(2)));
        for (auto j = 0; j < par_num_array_dims[i]; ++j) {
            par_array_dims[i].push_back(static_cast<uint8_t>(reader.read_b(8)));
        }
        par_val[i] = resizeVector(par_num_array_dims[i], par_array_dims[i]);
        for (auto& d1 : par_val[i])
            for (auto& d2 : d1)
                for (auto& d3 : d2) d3 = types.toArray(par_type[i], reader);
    }
}

void AlgorithmParameters::write(core::Writer& writer) const {
    ArrayType types;
    std::cerr << "n_pars: " << n_pars;
    writer.write(n_pars, 4);
    for (auto i = 0; i < n_pars; ++i) {
        std::cerr << ", par_num_array_dims[" << i << "] : " << par_num_array_dims[i] << std::endl;
        writer.write(par_ID[i], 4);
        writer.write(static_cast<uint8_t>(par_type[i]), 8);
        writer.write(par_num_array_dims[i], 2);
        for (auto j = 0; j < par_num_array_dims[i]; ++j) {
            std::cerr << "\t par_array_dims[" << i << "][" << j << "] : " << par_array_dims[i][j];
            writer.write(par_array_dims[i][j], 8);
        }
        for (auto j : par_val[i])
            for (auto k : j)
                for (auto l : k) types.toFile(par_type[i], l, writer);
    }
}
size_t AlgorithmParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

std::vector<std::vector<std::vector<std::vector<uint8_t>>>> AlgorithmParameters::resizeVector(
    uint8_t num_array_dims, std::vector<uint8_t> array_dims) {
    if (num_array_dims > array_dims.size()) return std::vector<std::vector<std::vector<std::vector<uint8_t>>>>();
    uint8_t j = 1;
    uint8_t k = 1;
    uint8_t l = 1;
    j = array_dims.at(0) + 1;
    if (num_array_dims > 0) k = array_dims.at(1) + 1;
    if (num_array_dims == 2) l = array_dims.at(2) + 1;

    //      [j]        [k]         [l]      [type]
    std::vector<std::vector<std::vector<std::vector<uint8_t>>>> temp(
        j, std::vector<std::vector<std::vector<uint8_t>>>(
               k, std::vector<std::vector<uint8_t>>(l, std::vector<uint8_t>())));
    return temp;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

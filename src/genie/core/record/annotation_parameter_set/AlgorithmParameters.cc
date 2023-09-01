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

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {
AlgorithmParameters::AlgorithmParameters()
    : n_pars(0), par_ID{}, par_type{}, par_num_array_dims{}, par_array_dims{}, par_val{} {}

AlgorithmParameters::AlgorithmParameters(util::BitReader& reader) { read(reader); }

AlgorithmParameters::AlgorithmParameters(
    uint8_t n_pars, std::vector<uint8_t> par_ID, std::vector<core::DataType> par_type, std::vector<uint8_t> par_num_array_dims,
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
    par_ID.resize(n_pars);
    par_type.resize(n_pars);
    par_num_array_dims.resize(n_pars);

    arrayType types;

    for (auto i = 0; i < n_pars; ++i) {
        par_ID[i] = (static_cast<uint8_t>(reader.read_b(4)));
        par_type[i] = (static_cast<core::DataType>(reader.read_b(8)));
        par_num_array_dims[i] = (static_cast<uint8_t>(reader.read_b(2)));
        for (auto j = 0; j < par_num_array_dims[i]; ++j) {
            par_array_dims[i].push_back(static_cast<uint8_t>(reader.read_b(8)));
        }
        if (par_num_array_dims[i] == 0)
            par_val[i].resize(1,
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 1)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(1, std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 2)
            par_val[i].resize(par_array_dims[i][0], std::vector<std::vector<std::vector<uint8_t>>>(
                                                        par_array_dims[i][1], std::vector<std::vector<uint8_t>>(1)));
        if (par_num_array_dims[i] == 3)
            par_val[i].resize(par_array_dims[i][0],
                              std::vector<std::vector<std::vector<uint8_t>>>(
                                  par_array_dims[i][1], std::vector<std::vector<uint8_t>>(par_array_dims[i][2])));
        for (auto& d1 : par_val[i])
            for (auto& d2 : d1)
                for (auto& d3 : d2) d3 = types.toArray(par_type[i], reader);
    }
}

void AlgorithmParameters::write(core::Writer& writer) const {
    arrayType types;
    writer.write(n_pars, 4);
    for (auto i = 0; i < n_pars; ++i) {
        writer.write(par_ID[i], 4);
        writer.write(static_cast<uint8_t>(par_type[i]), 8);
        writer.write(par_num_array_dims[i], 2);
        for (auto j = 0; j < par_num_array_dims[i]; ++j) {
            writer.write(par_array_dims[i][j], 8);
        }
        if (par_num_array_dims[i] == 0) {
            auto writeVal = par_val[0][0][0][i];
            types.toFile(par_type[i], writeVal, writer);
        }
        if (par_num_array_dims[i] == 1)
            for (auto j = 0; j < par_array_dims[i][0]; ++j) types.toFile(par_type[i], par_val[0][0][i][j], writer);

        if (par_num_array_dims[i] == 2)
            for (auto par_d1 : par_val[i])
                for (auto j = 0; j < par_array_dims[i][0]; ++j)
                    for (auto k = 0; k < par_array_dims[i][1]; ++k)
                        types.toFile(par_type[i], par_val[0][i][j][k], writer);
        if (par_num_array_dims[i] == 3)
            for (auto par_d1 : par_val[i])
                for (auto j = 0; j < par_array_dims[i][0]; ++j)
                    for (auto k = 0; k < par_array_dims[i][1]; ++k)
                        for (auto l = 0; l < par_array_dims[i][2]; ++l)
                            types.toFile(par_type[i], par_val[i][j][k][l], writer);
    }
}
size_t AlgorithmParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

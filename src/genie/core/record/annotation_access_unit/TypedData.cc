/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "TypedData.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {
void TypedData::convertToTypedData(CustomType value) {
    uint64_t n_elements = 1;
    data_block.resize(n_elements);
    data_block.at(0) = value;
}

void TypedData::convertToTypedData(std::vector<CustomType> matrix) {
    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }
    data_block.resize(n_elements);
    uint64_t index = 0;
    for (uint32_t k = 0; k < array_dims[2]; ++k) {
        data_block[index] = matrix.at(k);
    }
}

void TypedData::convertToTypedData(std::vector<std::vector<CustomType>> matrix) {
    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }
    data_block.resize(n_elements);
    uint64_t index = 0;
    for (uint32_t j = 0; j < array_dims[1]; ++j)
        for (uint32_t k = 0; k < array_dims[2]; ++k) {
            data_block[index] = matrix.at(j).at(k);
        }
}

void TypedData::convertToTypedData(std::vector<std::vector<std::vector<CustomType>>> matrix) {
    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }
    data_block.resize(n_elements);
    uint64_t index = 0;
    for (uint32_t i = 0; i < array_dims[0]; ++i)
        for (uint32_t j = 0; j < array_dims[1]; ++j)
            for (uint32_t k = 0; k < array_dims[2]; ++k) {
                data_block[index] = matrix.at(i).at(j).at(k);
            }
}
}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "typed_data.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace access_unit {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

void TypedData::writeElement(std::vector<CustomType> matrixRow) {
    ArrayType arrayType;
    for (auto elem : matrixRow) arrayType.toFile(data_type_ID, elem, writer);
}

void TypedData::convertToTypedData(util::BitReader& reader) {
    ArrayType arrayType;
    uint64_t n_elements = 1;
    for (uint8_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
        n_elements *= array_dims[idx_i];
    }
    arrayType.toFile(data_type_ID, reader, writer, n_elements);
}

void TypedData::convertToTypedData(CustomType value) {
    ArrayType arrayType;
    arrayType.toFile(data_type_ID, value, writer);
}

void TypedData::convertToTypedData(std::vector<CustomType> matrix) {
    UTILS_DIE_IF(array_dims[0] != matrix.size(), "matrix size does not match n_elements [0]!");

    uint64_t n_elements = 1;
    for (uint8_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
        n_elements *= array_dims[idx_i];
    }
    ArrayType arrayType;
    for (uint64_t idx_i = 0; idx_i < matrix.size(); ++idx_i) arrayType.toFile(data_type_ID, matrix.at(idx_i), writer);
}

void TypedData::convertToTypedData(std::vector<std::vector<CustomType>> matrix) {
    UTILS_DIE_IF(array_dims[0] != matrix.size(), "matrix size does not match n_elements [0]!");
    UTILS_DIE_IF(array_dims[1] != matrix.at(0).size(), "matrix size does not match n_elements [1]!");

    uint64_t n_elements = 1;
    for (uint8_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
        n_elements *= array_dims[idx_i];
    }
    ArrayType arrayType;

    for (uint32_t idx_j = 0; idx_j < matrix.size(); ++idx_j)
        for (uint32_t idx_k = 0; idx_k < matrix.at(0).size(); ++idx_k) {
            arrayType.toFile(data_type_ID, matrix.at(idx_j).at(idx_k), writer);
        }
}

void TypedData::convertToTypedData(std::vector<std::vector<std::vector<CustomType>>> matrix) {
    UTILS_DIE_IF(array_dims[0] != matrix.size(), "matrix size does not match n_elements [0]!"+std::to_string(array_dims[0]));
    UTILS_DIE_IF(array_dims[1] != matrix.at(0).size(),
                 "matrix size does not match n_elements [1]!" + std::to_string(array_dims[1]));
    UTILS_DIE_IF(array_dims[2] != matrix.at(0).at(0).size(),
                 "matrix size does not match n_elements [2]!" + std::to_string(array_dims[2]));

    uint64_t n_elements = 1;
    for (uint8_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
        n_elements *= array_dims[idx_i];
    }

    ArrayType arrayType;

    for (uint32_t idx_i = 0; idx_i < matrix.size(); ++idx_i)
        for (uint32_t idx_j = 0; idx_j < matrix.at(0).size(); ++idx_j)
            for (uint32_t idx_k = 0; idx_k < matrix.at(0).at(0).size(); ++idx_k) {
                arrayType.toFile(data_type_ID, matrix.at(idx_i).at(idx_j).at(idx_k), writer);
            }
}

void TypedData::write(core::Writer& outputWriter) const {
  outputWriter.Write(static_cast<uint8_t>(data_type_ID), 8);
    outputWriter.Write(num_array_dims, 2);
    uint64_t n_elements = 1;
    for (uint64_t idx_i = 0; idx_i < num_array_dims; ++idx_i) {
      outputWriter.Write(array_dims[idx_i], 32);
        n_elements = n_elements * array_dims[idx_i];
    }

    if (!compressedDataStream.str().empty()) {
        bool encoded = true;
        outputWriter.Write(encoded, 1);
        auto size = compressedDataStream.str().size();
        outputWriter.Write(size, 32);
        outputWriter.Write(
            const_cast<std::stringstream*>(&compressedDataStream));
    } else {
        bool encoded = false;
        outputWriter.Write(encoded, 1);
        outputWriter.Write(const_cast<std::stringstream*>(&dataStream));
    }
    outputWriter.Flush();
}

}  // namespace annotation
}  // namespace access_unit
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

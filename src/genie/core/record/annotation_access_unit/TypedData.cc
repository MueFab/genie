/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "TypedData.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

// ---------------------------------------------------------------------------------------------------------------------

void TypedData::writeElement(std::vector<CustomType> matrixRow) {
    ArrayType arrayType;
    for (auto elem : matrixRow) arrayType.toFile(data_type_ID, elem, writer);
}

void TypedData::convertToTypedData(util::BitReader& reader) {
    ArrayType arrayType;
    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
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
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }
    ArrayType arrayType;
    for (uint64_t i = 0; i < matrix.size(); ++i) arrayType.toFile(data_type_ID, matrix.at(i), writer);
}

void TypedData::convertToTypedData(std::vector<std::vector<CustomType>> matrix) {
    UTILS_DIE_IF(array_dims[0] != matrix.size(), "matrix size does not match n_elements [0]!");
    UTILS_DIE_IF(array_dims[1] != matrix.at(0).size(), "matrix size does not match n_elements [1]!");

    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }
    ArrayType arrayType;

    for (uint32_t j = 0; j < matrix.size(); ++j)
        for (uint32_t k = 0; k < matrix.at(0).size(); ++k) {
            arrayType.toFile(data_type_ID, matrix.at(j).at(k), writer);
        }
}

void TypedData::convertToTypedData(std::vector<std::vector<std::vector<CustomType>>> matrix) {
    UTILS_DIE_IF(array_dims[0] != matrix.size(), "matrix size does not match n_elements [0]!");
    //UTILS_DIE_IF(array_dims[1] != matrix.at(0).size(), "matrix size does not match n_elements [1]!");
    //UTILS_DIE_IF(array_dims[2] != matrix.at(0).at(0).size(), "matrix size does not match n_elements [2]!");

    uint64_t n_elements = 1;
    for (uint8_t i = 0; i < num_array_dims; ++i) {
        n_elements *= array_dims[i];
    }

    ArrayType arrayType;

    for (uint32_t i = 0; i < matrix.size(); ++i)
        for (uint32_t j = 0; j < matrix.at(0).size(); ++j)
            for (uint32_t k = 0; k < matrix.at(0).at(0).size(); ++k) {
                arrayType.toFile(data_type_ID, matrix.at(i).at(j).at(k), writer);
            }
}

void TypedData::write(core::Writer& outputWriter) const {
    outputWriter.write(static_cast<uint8_t>(data_type_ID), 8);
    outputWriter.write(num_array_dims, 2);
    uint64_t n_elements = 1;
    for (uint64_t i = 0; i < num_array_dims; ++i) {
        outputWriter.write(array_dims[i], 32);
        n_elements = n_elements * array_dims[i];
    }

    if (!compressedDataStream.str().empty()) {
    //    outputWriter.write(compressedDataStream.str().size(), 32);
        outputWriter.write(const_cast<std::stringstream*>(&compressedDataStream));
    } else {
        outputWriter.write(const_cast<std::stringstream*>(&dataStream));
    }
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

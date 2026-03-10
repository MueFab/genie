/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/ser/encoder.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace ser {

SEREncoder::SEREncoder()
    : order(false) {
}

void SEREncoder::encode() {
    // Use base class input/output storage
    if (inputs.empty()) {
        throw std::runtime_error("SEREncoder: No input data set");
    }

    auto& inputData = inputs[0];
    const auto& arrayDims = inputData.getArrayDims();
    const size_t totalElements = calculateTotalElements(arrayDims);

    // Create output TypedData objects
    outputs.clear();

    // Output 0: symbols (same type as input, flattened to 1D)
    std::vector<uint32_t> symbolsDims = {static_cast<uint32_t>(totalElements)};
    outputs.emplace_back(
        inputData.getDataTypeID(),
        1,  // num_array_dims = 1
        symbolsDims);

    // Output 1: dimensions (uint32_t, 1D array with size = num_array_dims)
    std::vector<uint32_t> dimensionsDims = {static_cast<uint32_t>(arrayDims.size())};
    outputs.emplace_back(
        core::DataType::UINT32,
        1,  // num_array_dims = 1
        dimensionsDims);

    // Get input data stream
    auto& inputStream = const_cast<core::record::annotation_access_unit::TypedData&>(inputData).getdata();
    const std::string inputStr = inputStream.str();
    const uint8_t* inputBytes = reinterpret_cast<const uint8_t*>(inputStr.data());

    // Determine element size based on data type
    size_t elementSize = 0;
    bool isStringType = false;
    
    switch (inputData.getDataTypeID()) {
        case core::DataType::UINT8:
        case core::DataType::INT8:
            elementSize = 1;
            break;
        case core::DataType::UINT16:
        case core::DataType::INT16:
            elementSize = 2;
            break;
        case core::DataType::UINT32:
        case core::DataType::INT32:
        case core::DataType::FLOAT:
            elementSize = 4;
            break;
        case core::DataType::UINT64:
        case core::DataType::INT64:
            elementSize = 8;
            break;
        case core::DataType::STRING:
            isStringType = true;
            elementSize = 0; // Variable size
            break;
        default:
            throw std::runtime_error("SEREncoder: Unsupported data type");
    }

    // Serialize data based on order parameter
    auto& symbolsStream = outputs[0].getDataStream();

    if (isStringType) {
        // Handle variable-length strings
        if (!order) {
            serializeStringsInOrder(inputBytes, inputStr.size(), arrayDims, symbolsStream);
        } else {
            serializeStringsReverseOrder(inputBytes, inputStr.size(), arrayDims, symbolsStream);
        }
    } else {
        // Handle fixed-size elements
        if (!order) {
            serializeInOrder(inputBytes, elementSize, arrayDims, symbolsStream);
        } else {
            serializeReverseOrder(inputBytes, elementSize, arrayDims, symbolsStream);
        }
    }

    // Write dimensions to output[1] directly
    std::vector<std::vector<uint8_t>> dimensionBytes;
    for (const auto& dim : arrayDims) {
      dimensionBytes.emplace_back(std::vector<uint8_t>(4));  // Add padding for next dimension
      memcpy(&dimensionBytes.back()[0], &dim, 4);  // Write dimension value in little-endian
    }
    outputs[1].writeElement(dimensionBytes);
}

void SEREncoder::decode() {
    throw std::runtime_error("SEREncoder: decode() not implemented");
}

size_t SEREncoder::calculateTotalElements(const std::vector<uint32_t>& dims) {
    if (dims.empty()) {
        return 0;
    }

    size_t total = 1;
    for (const auto& dim : dims) {
        total *= dim;
    }
    return total;
}

void SEREncoder::serializeInOrder(const uint8_t* data, size_t elementSize,
                                  const std::vector<uint32_t>& dims,
                                  std::stringstream& output) {
    // For normal order, data is already in the correct layout (row-major)
    // Just copy all elements sequentially
    size_t totalElements = calculateTotalElements(dims);
    output.write(reinterpret_cast<const char*>(data), totalElements * elementSize);
}

void SEREncoder::serializeReverseOrder(const uint8_t* data, size_t elementSize,
                                       const std::vector<uint32_t>& dims,
                                       std::stringstream& output) {
    // For reverse order, we need to transpose the data
    // This means iterating through dimensions in reverse order

    size_t numDims = dims.size();
    if (numDims == 0) {
        return;
    }

    if (numDims == 1) {
        // For 1D, order doesn't matter
        output.write(reinterpret_cast<const char*>(data), dims[0] * elementSize);
        return;
    }

    // Calculate strides for each dimension
    std::vector<size_t> strides(numDims);
    strides[numDims - 1] = 1;
    for (int i = numDims - 2; i >= 0; --i) {
        strides[i] = strides[i + 1] * dims[i + 1];
    }

    // Create index array
    std::vector<uint32_t> indices(numDims, 0);

    // Recursive function to iterate in reverse dimension order
    std::function<void(size_t)> iterate = [&](size_t dim) {
        if (dim == numDims) {
            // Calculate linear index
            size_t linearIndex = 0;
            for (size_t d = 0; d < numDims; ++d) {
                linearIndex += indices[d] * strides[d];
            }

            // Write element
            const uint8_t* element = data + (linearIndex * elementSize);
            output.write(reinterpret_cast<const char*>(element), elementSize);
            return;
        }

        // Iterate from last dimension to first
        size_t actualDim = numDims - 1 - dim;
        for (uint32_t i = 0; i < dims[actualDim]; ++i) {
            indices[actualDim] = i;
            iterate(dim + 1);
        }
    };

    iterate(0);
}

void SEREncoder::serializeStringsInOrder(const uint8_t* data, size_t dataSize,
                                        const std::vector<uint32_t>& dims,
                                        std::stringstream& output) {
    // For strings in normal order, just copy the data as-is
    // Each string is already null-terminated in the input
    output.write(reinterpret_cast<const char*>(data), dataSize);
}

void SEREncoder::serializeStringsReverseOrder(const uint8_t* data, size_t dataSize,
                                             const std::vector<uint32_t>& dims,
                                             std::stringstream& output) {
    size_t numDims = dims.size();
    if (numDims == 0) {
        return;
    }

    if (numDims == 1) {
        // For 1D, order doesn't matter
        output.write(reinterpret_cast<const char*>(data), dataSize);
        return;
    }

    // First, parse all strings into a vector to enable random access
    std::vector<const uint8_t*> stringPointers;
    std::vector<size_t> stringLengths;
    
    const uint8_t* current = data;
    const uint8_t* end = data + dataSize;
    
    while (current < end) {
        stringPointers.push_back(current);
        const uint8_t* stringEnd = current;
        while (stringEnd < end && *stringEnd != '\0') {
            ++stringEnd;
        }
        if (stringEnd < end) {
            ++stringEnd; // Include the null terminator
        }
        stringLengths.push_back(stringEnd - current);
        current = stringEnd;
    }

    // Calculate strides for each dimension
    std::vector<size_t> strides(numDims);
    strides[numDims - 1] = 1;
    for (int i = numDims - 2; i >= 0; --i) {
        strides[i] = strides[i + 1] * dims[i + 1];
    }

    // Create index array
    std::vector<uint32_t> indices(numDims, 0);

    // Recursive function to iterate in reverse dimension order
    std::function<void(size_t)> iterate = [&](size_t dim) {
        if (dim == numDims) {
            // Calculate linear index
            size_t linearIndex = 0;
            for (size_t d = 0; d < numDims; ++d) {
                linearIndex += indices[d] * strides[d];
            }

            // Write string element
            if (linearIndex < stringPointers.size()) {
                output.write(reinterpret_cast<const char*>(stringPointers[linearIndex]), 
                           stringLengths[linearIndex]);
            }
            return;
        }

        // Iterate from last dimension to first
        size_t actualDim = numDims - 1 - dim;
        for (uint32_t i = 0; i < dims[actualDim]; ++i) {
            indices[actualDim] = i;
            iterate(dim + 1);
        }
    };

    iterate(0);
}

core::record::annotation_parameter_set::AlgorithmParameters SERParameters::convertToAlgorithmParameters() const {
    uint8_t n_pars = 1;
    std::vector<uint8_t> par_ID{ 1 };
    const std::vector<core::DataType> par_type{ core::DataType::BOOL };
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    std::vector<uint8_t> values{ static_cast<uint8_t>(order ? 1 : 0) };

    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));

    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto i = 0; i < n_pars; ++i) {
        if (par_type.at(i) == core::DataType::BOOL) {
            par_val.push_back(core::record::annotation_parameter_set::parameterToVector<uint8_t>(
                { values.at(i) }, par_type.at(i), par_num_array_dims.at(i), par_array_dims.at(i)));
        }
    }

    return core::record::annotation_parameter_set::AlgorithmParameters(n_pars, par_ID, par_type,
                                                         par_num_array_dims, par_array_dims, par_val);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace ser
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

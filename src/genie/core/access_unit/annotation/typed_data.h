/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_TYPED_DATA_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_TYPED_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <sstream>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

using CustomType = std::vector<uint8_t>;

class TypedData {
 public:
    TypedData(core::DataType TypeId, uint8_t numArrayDims, std ::vector<uint32_t> arrayDims)
        : data_type_ID(TypeId),
          num_array_dims(numArrayDims),
          array_dims(arrayDims),
          dataStream{},
          // DEPRECATED: writer{&dataStream},
          compressedDataStream{} {}

    TypedData()
        : data_type_ID(core::DataType::UINT8),
          num_array_dims(0),
          array_dims{},
          dataStream{},
          // DEPRECATED: writer{&dataStream},
          compressedDataStream{} {}

    TypedData& operator=(const TypedData& other) {
        if (this != &other) {
            data_type_ID = other.data_type_ID;
            num_array_dims = other.num_array_dims;
            array_dims = other.array_dims;
            dataStream << other.dataStream.rdbuf();
            compressedDataStream << other.compressedDataStream.rdbuf();
        }
        return *this;
    }

    TypedData(TypedData& other) {
        data_type_ID = other.data_type_ID;
        num_array_dims = other.num_array_dims;
        array_dims = other.array_dims;
        dataStream << other.dataStream.rdbuf();
        compressedDataStream << other.compressedDataStream.rdbuf();
    }

    TypedData(TypedData&&) = default;

    void set(core::DataType TypeId, uint8_t numArrayDims, std ::vector<uint32_t> arrayDims) {
        data_type_ID = TypeId;
        num_array_dims = numArrayDims;
        array_dims = arrayDims;
    }

    void setArrayDim0(uint32_t value) { array_dims.at(0) = value; }
    void setArrayDim1(uint32_t value) {
        if (array_dims.size() > 1) array_dims.at(1) = value;
    }

    void convertToTypedData(util::BitReader& reader);

    void writeElement(std::vector<CustomType> matrixRow);

    void convertToTypedData(CustomType value);
    void convertToTypedData(std::vector<CustomType> matrix);
    void convertToTypedData(std::vector<std::vector<CustomType>> matrix);
    void convertToTypedData(std::vector<std::vector<std::vector<CustomType>>> matrix);

    // DEPRECATED: Internal method used writer member - commented out
    // void convertToTypedData(std::vector<std::vector<CustomType>> matrix);

    // DEPRECATED: Internal method used writer member - commented out
    // void convertToTypedData(std::vector<std::vector<std::vector<CustomType>>> matrix);

    std::stringstream& getDataStream() { return dataStream; }

    std::stringstream& getCompresseddata() { return compressedDataStream; }

    void write(util::BitWriter& writer) const;

 private:
    core::DataType data_type_ID;
    uint8_t num_array_dims;
    std::vector<uint32_t> array_dims;
    std::vector<CustomType> data_block;
    std::stringstream dataStream;
    // util::BitWriter writer{dataStream};
    std::stringstream compressedDataStream;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::access_unit::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_TYPED_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

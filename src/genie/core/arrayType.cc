/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "arrayType.h"
#include <cstring>
#include <iostream>
#include "genie/util/bit_reader.h"

namespace genie {
namespace core {

std::string ArrayType::toString(core::DataType type, std::vector<uint8_t> bytearray) const {
    std::string temp;

    switch (type) {
        case core::DataType::CHAR:
        case core::DataType::BOOL:
        case core::DataType::UINT8:
            temp = std::to_string(bytearray[0]);
            break;
        case core::DataType::INT8: {
            int8_t value;
            memcpy(&value, &bytearray[0], 1);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::INT16: {
            int16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::UINT16: {
            uint16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::INT32: {
            int32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::UINT32: {
            uint32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::INT64: {
            int64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::UINT64: {
            uint64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::FLOAT: {  // decided to treat a float the same as u32
            uint32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case core::DataType::DOUBLE: {  // double
            uint64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        default: {  // case 0
            temp += '"';
            if (bytearray.size() > 0) {
                for (size_t i = 0; i < bytearray.size(); ++i) temp += bytearray[i];
            }
            temp += '"';
            break;
        }
    }
    return temp;
}

uint8_t ArrayType::getDefaultBitsize(core::DataType type) const {
    if (type == DataType::STRING) return 0;
    if (type == DataType::CHAR || type == DataType::INT8 || type == DataType::UINT8) return 8;
    if (type == DataType::BOOL) return 1;
    if (type == DataType::INT16 || type == DataType::UINT16) return 16;
    if (type == DataType::UINT32 || type == DataType::INT32 || type == DataType::FLOAT) return 32;
    if (type == DataType::UINT64 || type == DataType::INT64 || type == DataType::DOUBLE) return 64;
    return 0;
}

uint64_t ArrayType::getDefaultValue(core::DataType type) const {
    switch (type) {
        case core::DataType::STRING:
        case core::DataType::CHAR:
        case core::DataType::BOOL:
            return 0;
        case core::DataType::INT8:
            return 0x80;
        case core::DataType::UINT8:
            return 0xFF;
        case core::DataType::INT16:
            return 0x8000;
        case core::DataType::UINT16:
            return 0xFFFF;
        case core::DataType::INT32:
            return 0x80000000;
        case core::DataType::UINT32:
            return 0xFFFFFFFF;
        case core::DataType::INT64:
            return 0x8000000000000000;
        case core::DataType::UINT64:
            return 0xFFFFFFFFFFFFFFFF;
        case core::DataType::FLOAT:
            return 0x7F800001;
        case core::DataType::DOUBLE:
            return 0x7FFF000000000001;
        default:
            break;
    }
    return 0;
}

std::vector<uint8_t> ArrayType::toArray(DataType type, util::BitReader& reader) const {
    std::vector<uint8_t> byteArray;
    switch (type) {
        case DataType::BOOL:
            byteArray.push_back(static_cast<uint8_t>(reader.ReadBits(1)));
            break;
        case DataType::CHAR:
        case DataType::UINT8:
            byteArray.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
            break;
        case DataType::INT8: {
            uint8_t readValue = static_cast<uint8_t>(reader.ReadBits(8));
             byteArray.resize(1);
            memcpy(&byteArray[0], &readValue, 1);
            break;
        }
        case DataType::INT16: {
            uint16_t readValue = static_cast<uint16_t>(reader.ReadBits(16));
            byteArray.resize(2);
            memcpy(&byteArray[0], &readValue, 2);
            break;
        }
        case DataType::UINT16: {
            uint16_t temp;
            temp = static_cast<uint16_t>(reader.ReadBits(16));
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case DataType::INT32: {
            uint32_t readValue = static_cast<uint32_t>(reader.ReadBits(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &readValue, 4);
            break;
        }
        case DataType::UINT32: {
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.ReadBits(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case DataType::INT64: {
            int64_t temp;
            temp = static_cast<int64_t>(reader.ReadBits(64));
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case DataType::DOUBLE:
        case DataType::UINT64: {
            uint64_t temp;
            temp = static_cast<uint64_t>(reader.ReadBits(64));
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case DataType::FLOAT: {  // decided to treat a float the same as u32
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.ReadBits(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        default:  // case STRING
            uint8_t read = static_cast<uint8_t>(reader.ReadBits(8));

            while (read != 0) {
                byteArray.push_back(read);
                read = static_cast<uint8_t>(reader.ReadBits(8));
            }
            break;
    }

    return byteArray;
}

void ArrayType::toFile(core::DataType type, std::vector<uint8_t> bytearray, core::Writer& writer) const {
    if (type == core::DataType::BOOL) {
        writer.write(bytearray[0], 1);
    } else if (type == core::DataType::STRING) {
        std::string stringOut;
        if (bytearray.size() > 0)
            for (auto byte : bytearray) stringOut += byte;
        writer.write(stringOut);
        writer.write(0, 8, true);
    } else {
        uint64_t writeValue = 0;
        if (bytearray.size() == 0) {
            //        std::cerr << "nothing to write\n";
        } else {
            uint8_t byteSize = getDefaultBitsize(type);
            byteSize /= 8;
            if (byteSize == 0) byteSize = 1;

            for (auto i = 0; (i < byteSize); ++i) writeValue += static_cast<uint64_t>(bytearray[i]) << (i * 8);
            writer.write(writeValue, static_cast<uint8_t>(byteSize) * 8);
        }
    }
}

void ArrayType::toFile(core::DataType type, util::BitReader& reader, core::Writer& writer, uint64_t number = 1) const {
    if (type == core::DataType::STRING) {
        for (uint64_t i = 0; i < number; ++i) {
            std::string temp;
            reader.ReadAlignedStringTerminated(temp);
            writer.write(temp);
            writer.write(0, 8, true);
        }
    } else {
        if (number > 0) {
            //        uint64_t writeValue = 0;
            uint8_t byteSize = getDefaultBitsize(type);
            if (byteSize == 0) byteSize = 1;
            for (uint64_t n = 0; n < number; ++n) writer.write(reader.ReadBits(byteSize), byteSize);
        }
    }
}

std::vector<uint8_t> ArrayType::toArray(DataType type, uint64_t value) const {
    std::vector<uint8_t> returnValue;
    auto NrOfBytes = (getDefaultBitsize(type) + 7) / 8;
    returnValue.resize(NrOfBytes);
    for (auto i = 0; i < NrOfBytes; ++i) {
        returnValue[i] = value & 0xFF;
        value = value >> 8;
    }
    return returnValue;
}

}  // namespace core
}  // namespace genie

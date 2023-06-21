#include "arrayType.h"
#include <cstring>
#include "genie/util/bitreader.h"

namespace genie {
namespace core {

std::string arrayType::toString(uint8_t type, std::vector<uint8_t> bytearray) const {
    std::string temp;

    switch (type) {
        case 1:
        case 2:
        case 4:
            temp = std::to_string(bytearray[0]);
            break;
        case 3: {
            int8_t value;
            memcpy(&value, &bytearray[0], 1);
            temp = std::to_string(value);
            break;
        }
        case 5: {
            int16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case 6: {
            uint16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case 7: {
            int32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case 8: {
            uint32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case 9: {
            int64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case 10: {
            uint64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case 11: {  // decided to treat a float the same as u32
            uint32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case 12: {  // double
            uint64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        default:  // csae 0

            temp += '"';
            if (bytearray.size() > 0)
                for (auto i = 0; i < bytearray.size(); ++i) temp += bytearray[i];
            temp += '"';
            break;
    }
    return temp;
}

uint8_t arrayType::getDefaultBitsize(uint8_t type) {
    if (type == 0 || type == 1 || type == 3 || type == 4) return 8;
    if (type == 2) return 1;
    if (type == 5 || type == 6) return 16;
    if (type == 7 || type == 8 || type == 11) return 32;
    if (type == 9 || type == 10 || type == 12) return 64;
    return 0;
}

uint64_t arrayType::getDefaultValue(uint8_t type) {
    switch (type) {
        case 0:
        case 1:
        case 2:
            return 0;
        case 3:
            return 0x80;
        case 4:
            return 0xFF;
        case 5:
            return 0x8000;
        case 6:
            return 0xFFFF;
        case 7:
            return 0x80000000;
        case 8:
            return 0xFFFFFFFF;
        case 9:
            return 0x8000000000000000;
        case 10:
            return 0xFFFFFFFFFFFFFFFF;
        case 11:
            return 0x7F800001;
        case 12:
            return 0x7FFF000000000001;
        default:
            break;
    }
    return 0;
}

std::vector<uint8_t> arrayType::toArray(uint8_t type, util::BitReader& reader) {
    std::vector<uint8_t> byteArray;
    switch (type) {
        case 2:
            byteArray.push_back(static_cast<uint8_t>(reader.read_b(1)));
            break;
        case 1:
        case 4:
            byteArray.push_back(static_cast<uint8_t>(reader.read_b(8)));
            break;
        case 3:
            byteArray.push_back(static_cast<uint8_t>(reader.read_b(8)));
            break;
        case 5: {
            int16_t temp;
            temp = static_cast<int16_t>(reader.read_b(16));
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 6: {
            uint16_t temp;
            temp = static_cast<uint16_t>(reader.read_b(16));
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 7: {
            int32_t temp;
            temp = static_cast<int32_t>(reader.read_b(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 8: {
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.read_b(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 9: {
            int64_t temp;
            temp = static_cast<int64_t>(reader.read_b(64));
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 12:
        case 10: {
            uint64_t temp;
            temp = static_cast<uint64_t>(reader.read_b(64));
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 11: {  // decided to treat a float the same as u32
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.read_b(32));
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        default:  // case 0
            uint8_t read = static_cast<uint8_t>(reader.read_b(8));

            while (read != 0) {
                byteArray.push_back(read);
                read = static_cast<uint8_t>(reader.read_b(8));
            };
            break;
    }

    return byteArray;
}

void arrayType::toFile(uint8_t type, std::vector<uint8_t> bytearray, core::Writer& writer) const {
    if (type == 2) {
        writer.write(bytearray[0], 1);
    } else if (type == 0) {
        for (auto i = 0; i < bytearray.size(); ++i) writer.write(bytearray[i], 8);
        writer.write(0, 8);
    } else {
        for (auto i = bytearray.size(); i > 0; --i) writer.write(bytearray[i - 1], 8);
    }
}

}  // namespace core
}  // namespace genie

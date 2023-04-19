#include "arrayType.h"
#include "genie/util/bitreader.h"

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

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
        default:  // csae 0
            temp.resize(bytearray.size() + 2);
            temp += '"';
            for (auto i = 0; i < bytearray.size(); ++i) temp += bytearray[i];
            temp += '"';
            break;
    }
    return temp;
}

std::vector<uint8_t> arrayType::toArray(uint8_t type, util::BitReader& reader) {
    std::vector<uint8_t> byteArray;
    switch (type) {
        case 1:
        case 2:
        case 4:
            byteArray.push_back(static_cast<uint8_t>(reader.read_b(8)));  // reader.readBypassBE<uint8_t>());
            break;
        case 3:
            byteArray.push_back(
                static_cast<uint8_t>(reader.read_b(8)));  // static_cast<uint8_t>(reader.readBypassBE<int8_t>()));
            break;
        case 5: {
            int16_t temp;
            temp = static_cast<int16_t>(reader.read_b(16));//reader.readBypassBE<int16_t>();
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 6: {
            uint16_t temp;
            temp = static_cast<uint16_t>(reader.read_b(16));  // reader.readBypassBE<uint16_t>();
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 7: {
            int32_t temp;
            temp = static_cast<int32_t>(reader.read_b(32));  // reader.readBypassBE<int32_t>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 8: {
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.read_b(32));  // reader.readBypassBE<uint32_t>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 9: {
            int64_t temp;
            temp = static_cast<int64_t>(reader.read_b(64));  // reader.readBypassBE<int64_t>();
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 10: {
            uint64_t temp;
            temp = static_cast<uint64_t>(reader.read_b(64));  // reader.readBypassBE<uint64_t>();
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 11: {  // decided to treat a float the same as u32
            uint32_t temp;
            temp = static_cast<uint32_t>(reader.read_b(32));  // reader.readBypassBE<uint32_t>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        default:  // case 0
            uint8_t read = 0xF;
            do {
                read = static_cast<uint8_t>(reader.read_b(8));  // reader.readBypassBE<uint8_t>();
                if (read != 0) byteArray.push_back(read);
            } while (read != 0);
            break;
    }

    return byteArray;
}

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

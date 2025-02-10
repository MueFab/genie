/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>

#include "genie/annotation/json_attribute_parser.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void JsonAttributeParser::ParseInfoFields() {
    infoFieldsJSON.erase(std::remove(infoFieldsJSON.begin(), infoFieldsJSON.end(), '\n'), infoFieldsJSON.end());
    infoFieldsJSON.erase(std::remove(infoFieldsJSON.begin(), infoFieldsJSON.end(), ' '), infoFieldsJSON.end());
    infoFieldsJSON.erase(std::remove(infoFieldsJSON.begin(), infoFieldsJSON.end(), '\t'), infoFieldsJSON.end());
    infoFieldsJSON.erase(std::remove(infoFieldsJSON.begin(), infoFieldsJSON.end(), '"'), infoFieldsJSON.end());

    std::stringstream infoFieldsList;
    infoFieldsList << infoFieldsJSON.substr(1, infoFieldsJSON.size() - 2);
    std::string infoFieldString;
    while (std::getline(infoFieldsList, infoFieldString, '}')) {
        infoFieldString = infoFieldString.substr(1, infoFieldString.size() - 1);
        InfoField infoField;
        auto start = infoFieldString.find(':');
        auto end = infoFieldString.find(',');
        infoField.ID = infoFieldString.substr(start + 1, end - start - 1);
        start = infoFieldString.find(':', end + 1);
        end = infoFieldString.find(',', end + 1);
        infoField.Type = InterpretType(infoFieldString.substr(start + 1, end - start - 1));
        start = infoFieldString.find(':', end + 1);
        end = infoFieldString.find(',', end + 1);
        infoField.Number = InterpretSize(infoFieldString.substr(start + 1, end - start - 1));
        infoFields.push_back(infoField);
    }
}

genie::core::DataType JsonAttributeParser::InterpretType(std::string type) const {
  /*  if (type == "STRING") return genie::core::DataType::STRING;
    if (type == "INTEGER") return genie::core::DataType::INT32;
    if (type == "FLOAT") return genie::core::DataType::FLOAT;
    if (type == "INT16") return genie::core::DataType::INT16;
    if (type == "UINT16") return genie::core::DataType::UINT16;
    if (type == "INT32") return genie::core::DataType::INT32;
    if (type == "UINT32") return genie::core::DataType::UINT32;*/
    return static_cast<genie::core::DataType>(stoi(type));
   // return genie::core::DataType::UINT64;
}

uint8_t JsonAttributeParser::InterpretSize(std::string size) const {
    auto str = size.c_str();
    if (std::isdigit(size[0])) return static_cast<uint8_t>(atoi(str));
    // size == '.'
    return 1;
}

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

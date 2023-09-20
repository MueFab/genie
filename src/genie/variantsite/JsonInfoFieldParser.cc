#include "VariantSiteParser.h"
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include <cctype>

#include "genie/variantsite/JsonInfoFieldParser.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

void JsonInfoFieldParser::ParseInfoFields() {
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

genie::core::DataType JsonInfoFieldParser::InterpretType(std::string type) const {
    if (type == "STRING") return genie::core::DataType::STRING;
    if (type == "INTEGER") return genie::core::DataType::INT32;
    if (type == "FLOAT")
        return genie::core::DataType::FLOAT;
    else
        return genie::core::DataType::UINT64;
}

uint8_t JsonInfoFieldParser::InterpretSize(std::string size) const {
    auto str = size.c_str();
    if (std::isdigit(size[0])) return static_cast<uint8_t>(atoi(str));
    // size == '.'
    return 1;
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

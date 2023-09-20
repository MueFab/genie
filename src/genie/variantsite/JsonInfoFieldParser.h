/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANT_SITE_JSONINFOFIELDPARSER_H_
#define SRC_GENIE_VARIANT_SITE_JSONINFOFIELDPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/arrayType.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class InfoField {
 public:
    std::string ID;
    genie::core::DataType Type;
    uint8_t Number;
};

//--------- file format ----------------//
// [
//     {
//     "ID" : "AA",
//     "Type" : "STRING",
//      "Number" : 1
//     }
// etc.
// ]
//--------------------------------------//
// Type can be: STRING, INTEGER, FLOAT
// Number can be: 1,2, .

class JsonInfoFieldParser {
 public:
    JsonInfoFieldParser(std::stringstream& jsonInfoFields) {
        infoFieldsJSON = jsonInfoFields.str();
        if (!infoFieldsJSON.empty()) ParseInfoFields();
    }

    std::vector<InfoField> getInfoFields() const { return infoFields; }
    InfoField getInfoField(uint8_t index) const { return infoFields[index]; }

 private:
    std::string infoFieldsJSON;
    std::vector<InfoField> infoFields;
    std::map<std::string, uint8_t> infoFieldType;

    void ParseInfoFields();
    genie::core::DataType InterpretType(std::string type) const;
    uint8_t InterpretSize(std::string size) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANT_SITE_JSONINFOFIELDPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

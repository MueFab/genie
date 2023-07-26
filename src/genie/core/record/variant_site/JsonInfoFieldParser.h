/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_SITE_JSONINFOFIELDPARSER_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_SITE_JSONINFOFIELDPARSER_H_

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

#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

class InfoField {
 public:
    std::string ID;
    uint8_t Type;
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
    uint8_t InterpretType(std::string type) const;
    uint8_t InterpretSize(std::string size) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_SITE_JSONINFOFIELDPARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

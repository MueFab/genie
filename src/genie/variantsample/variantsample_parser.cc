
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/variantsample/variantsample_parser.h"

#include <vector>

namespace genie::variant_sample {

    VariantSampleParser::VariantSampleParser(std::istream& _samples, std::vector<annotation::InfoField>& _fields,
        uint32_t _rowsPerTile)
        : sampleMGrecs(_samples),
        rowsPerTile(_rowsPerTile),
        numberOfColumns(0),
        infoFields(_fields),
        numberOfAttributes(0),
        startPos(0) {
        init();

        util::BitReader reader(sampleMGrecs);
        while (fillRecord(reader)) {
          attributes.add(sample.GetSampleAttributes().GetFields());
          numberOfColumns++;
        }
    }

    void VariantSampleParser::init() {
        uint16_t attributeID = 0;
        for (const auto& infoField : infoFields) {
            AttributeData attribute(static_cast<uint8_t>(infoField.ID.length()), infoField.ID,
                                    infoField.Type, infoField.Number, attributeID);
            attributeData[infoField.ID] = attribute;
            attributeID++;
        }

        variant_site::Attributes attr(rowsPerTile, attributeData);
        attributes = attr;

        numberOfAttributes = static_cast<uint16_t>(infoFields.size());
    }

bool VariantSampleParser::fillRecord(util::BitReader reader) {
      if (!sample.Read(reader))
        return false;
      return true;
    }

}  // namespace genie::variant_sample


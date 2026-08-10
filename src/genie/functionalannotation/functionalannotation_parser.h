/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FUNCTIONALANNOTATION_FUNCTIONALANNOTATION_PARSER_H_
#define SRC_GENIE_FUNCTIONALANNOTATION_FUNCTIONALANNOTATION_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

#include "genie/variantsite/attributes.h"
#include "genie/functionalannotation/descriptors.h"

#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/record/functional_annotation/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::functional_annotation {

//-------------------------------------------------------------------------------//

class FunctionalAnnotationParser {
 public:
    using AttributeData = genie::core::parameter::annotation::AttributeData;
    using InfoField = genie::annotation::InfoField;

    FunctionalAnnotationParser(std::istream& _annotation_MGrecs, std::vector<genie::annotation::InfoField>& _fields,
                              const std::vector<std::string>& _featureNames,
                              const std::vector<std::string>& _ontologyNames,
                              uint8_t _maxOntologiesPerRecord,
                              uint64_t _rowsPerTile);

    size_t getNumberOfRows() const { return numberOfRows; }

    variant_site::Attributes& getAttributes() { return attributes; }
    Descriptors& getDescriptors() { return descriptors; }

    uint64_t getNrOfTiles() { return descriptors.getTiles()[genie::core::AnnotDesc::STARTPOS].getTiles().size(); }

 private:
    genie::core::record::functional_annotation::Record functionalAnnotation;
    std::istream& annotationMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
    std::vector<InfoField> infoFields;
    std::map<std::string, uint8_t> infoFieldType;

    std::vector<util::BitWriter> fieldWriter;
    std::map<std::string, util::BitWriter> attrWriter;

    std::map<std::string, AttributeData> attributeData;
    variant_site::Attributes attributes;
    Descriptors descriptors;

    uint16_t numberOfAttributes;

    void init();
    bool fillRecord(util::BitReader reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::functional_annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FUNCTIONALANNOTATION_FUNCTIONALANNOTATION_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

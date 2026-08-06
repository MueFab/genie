/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_FUNCTIONAL_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_FUNCTIONAL_ANNOTATION_H_

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/annotation/json_attribute_parser.h"
#include "genie/annotation/accessunit_composer.h"

#include "genie/annotation/compressors.h"
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/access_unit/annotation/record.h"

// -----------------------------------------------------------------------------

namespace genie::annotation {
// ---------------------------------------------------------------------------------------------------------------------
struct FunctionalAnnotationUnits {
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;
};

class FunctionalAnnotation {
 public:
    FunctionalAnnotation() : maxOntologiesPerRecord(0), defaultTileSizeHeight(0) {}

    void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }
    void setTileSize(uint32_t _defaultTileSizeHeight) { defaultTileSizeHeight = _defaultTileSizeHeight; }

    void setInfoFields(std::string jsonFileName);

    void parseInfoTags(std::string& recordInputFileName);
    FunctionalAnnotationUnits parseFunctionalAnnotation(std::ifstream& inputfile);
    void setCompressors(Compressor& _compressors) { compressors = _compressors; }
    void setAnnotationSubtype(core::access_unit::annotation::AnnotationSubtype subtype) {
        annotationSubtype_ = subtype;
    }

 private:
    std::ifstream recordInput;
    Compressor compressors;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    std::vector<std::string> featureNames;
    std::vector<std::string> ontologyNames;
    uint8_t maxOntologiesPerRecord;
    const std::vector<core::AnnotDesc> descrList{
        core::AnnotDesc::SEQUENCEID,   core::AnnotDesc::STARTPOS,
        core::AnnotDesc::ENDPOS,       core::AnnotDesc::STRAND,
        core::AnnotDesc::LINKNAME,     core::AnnotDesc::LINKID,
        core::AnnotDesc::FEATURENAME,  core::AnnotDesc::FEATUREID,
        core::AnnotDesc::ONTOLOGYNAME, core::AnnotDesc::ONTOLOGYID};

    variant_site::AccessUnitComposer accessUnitcomposer;
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
    core::access_unit::annotation::AnnotationSubtype annotationSubtype_{
        core::access_unit::annotation::AnnotationSubtype::GFF};
};

}  // namespace genie::annotation

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_FUNCTIONAL_ANNOTATION_H_

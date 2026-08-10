/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_SITE_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_SITE_ANNOTATION_H_

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/annotation/json_attribute_parser.h"
#include "genie/annotation/accessunit_composer.h"

#include "genie/annotation/compressors.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/access_unit/annotation/record.h"
#include "genie/core/record/site/record.h"

// -----------------------------------------------------------------------------

namespace genie::annotation {
// ---------------------------------------------------------------------------------------------------------------------
struct SiteUnits {
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;
};

class SiteAnnotation {
 public:
    void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }
    void setTileSize(uint32_t _defaultTileSizeHeight) { defaultTileSizeHeight = _defaultTileSizeHeight; }

    void setInfoFields(std::string jsonFileName);

    void parseInfoTags(std::string& recordInputFileName);
    SiteUnits parseSite(std::ifstream& inputfile);
    void setCompressors(genie::annotation::Compressor& _compressors) { compressors = _compressors; }

 private:
    std::ifstream recordInput;
    genie::annotation::Compressor compressors;
    std::map<std::string, genie::core::record::site::Info_tag> infoTags;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    const std::vector<genie::core::AnnotDesc> descrList{
        genie::core::AnnotDesc::SEQUENCEID, genie::core::AnnotDesc::STARTPOS,       genie::core::AnnotDesc::STRAND,
        genie::core::AnnotDesc::NAME,       genie::core::AnnotDesc::DESCRIPTION,    genie::core::AnnotDesc::LINKNAME,
        genie::core::AnnotDesc::LINKID,     genie::core::AnnotDesc::DEPTH,          genie::core::AnnotDesc::SEQQUALITY,
        genie::core::AnnotDesc::MAPQUALITY, genie::core::AnnotDesc::MAPNUMQUALITY0, genie::core::AnnotDesc::REFERENCE,
        genie::core::AnnotDesc::ALTERN,     genie::core::AnnotDesc::FILTER};


    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::parameter::annotation::Record annotationParameterSet;
    std::vector<genie::core::access_unit::annotation::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
};

}  // namespace genie::annotation

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_SITE_ANNOTATION_H_

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SITE_ANNOTATION_ANNOTATION_H
#define GENIE_SITE_ANNOTATION_ANNOTATION_H

#include <fstream>
#include <map>
#include <sstream>
#include "genie/annotation/JsonAttributeParser.h"
#include "genie/core/constants.h"
#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"
#include "genie/variantsite/VariantSiteParser.h"

#include "genie/annotation/Compressors.h"
#include "genie/core/record/data_unit/record.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------
struct SiteUnits {
    core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record> annotationAccessUnit;
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
    std::map<std::string, genie::core::record::variant_site::Info_tag> infoTags;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    std::vector<genie::core::AnnotDesc> descrList{
        genie::core::AnnotDesc::SEQUENCEID, genie::core::AnnotDesc::STARTPOS,       genie::core::AnnotDesc::STRAND,
        genie::core::AnnotDesc::NAME,       genie::core::AnnotDesc::DESCRIPTION,    genie::core::AnnotDesc::LINKNAME,
        genie::core::AnnotDesc::LINKID,     genie::core::AnnotDesc::DEPTH,          genie::core::AnnotDesc::SEQQUALITY,
        genie::core::AnnotDesc::MAPQUALITY, genie::core::AnnotDesc::MAPNUMQUALITY0, genie::core::AnnotDesc::REFERENCE,
        genie::core::AnnotDesc::ALTERN,     genie::core::AnnotDesc::FILTER}        ;


    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_SITE_ANNOTATION_ANNOTATION_H

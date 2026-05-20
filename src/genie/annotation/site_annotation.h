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
#include "genie/core/constants.h"
#include "genie/variantsite/accessunit_composer.h"
#include "genie/variantsite/parameterset_composer.h"
#include "genie/variantsite/variantsite_parser.h"

#include "genie/annotation/compressors.h"
#include "genie/core/data_unit_record/record.h"
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
    void setCompressors(Compressor& _compressors) { compressors = _compressors; }

 private:
    std::ifstream recordInput;
    Compressor compressors;
    std::map<std::string, core::record::variant_site::Info_tag> infoTags;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    const std::vector<core::AnnotDesc> descrList{
        core::AnnotDesc::SEQUENCEID, core::AnnotDesc::STARTPOS,       core::AnnotDesc::STRAND,
        core::AnnotDesc::NAME,       core::AnnotDesc::DESCRIPTION,    core::AnnotDesc::LINKNAME,
        core::AnnotDesc::LINKID,     core::AnnotDesc::DEPTH,          core::AnnotDesc::SEQQUALITY,
        core::AnnotDesc::MAPQUALITY, core::AnnotDesc::MAPNUMQUALITY0, core::AnnotDesc::REFERENCE,
        core::AnnotDesc::ALTERN,     core::AnnotDesc::FILTER};


    variant_site::AccessUnitComposer accessUnitcomposer;
    core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_SITE_ANNOTATION_H_

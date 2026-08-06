/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_TRACK_DATA_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_TRACK_DATA_ANNOTATION_H_

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/annotation/accessunit_composer.h"
#include "genie/annotation/compressors.h"
#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/access_unit/annotation/record.h"

// -----------------------------------------------------------------------------

namespace genie::annotation {
// ---------------------------------------------------------------------------------------------------------------------
struct TrackUnits {
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;
};

class TrackDataAnnotation {
 public:
    TrackDataAnnotation() : defaultTileSizeHeight(0) {}

    void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }
    void setTileSize(uint32_t _defaultTileSizeHeight) { defaultTileSizeHeight = _defaultTileSizeHeight; }

    void setInfoFields(std::string jsonFileName);

    void parseInfoTags(std::string& recordInputFileName);
    TrackUnits parseTrack(std::ifstream& inputfile);
    void setCompressors(Compressor& _compressors) { compressors = _compressors; }
    void setAnnotationSubtype(core::access_unit::annotation::AnnotationSubtype subtype) {
        annotationSubtype_ = subtype;
    }

    uint64_t getNumberOfRecords() const { return numberOfRecords; }

 private:
    std::ifstream recordInput;
    Compressor compressors;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    const std::vector<core::AnnotDesc> descrList{
        core::AnnotDesc::SEQUENCEID,
        core::AnnotDesc::STARTPOS,
        core::AnnotDesc::ENDPOS,
        core::AnnotDesc::STRAND,
        core::AnnotDesc::LINKNAME,
        core::AnnotDesc::LINKID};

    variant_site::AccessUnitComposer accessUnitcomposer;
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
    uint64_t numberOfRecords{0};
    core::access_unit::annotation::AnnotationSubtype annotationSubtype_{
        core::access_unit::annotation::AnnotationSubtype::BED};
};

}  // namespace genie::annotation

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_TRACK_DATA_ANNOTATION_H_

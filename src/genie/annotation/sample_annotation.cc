/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/sample_annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/annotation/accessunit_composer.h"

#include "genie/core/array_type.h"
#include "genie/util/runtime_exception.h"

#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"

namespace genie {
namespace annotation {

void SampleAnnotation::parseInfoTags(std::string& recordInputFileName) {
  std::ifstream readForTags;
  readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
  util::BitReader bitreader(readForTags);
  std::vector<core::record::sample::SampleFields::Field> infoTag;
  core::record::sample::Record recs;
  while (recs.Read(bitreader)) {
    infoTag = recs.GetSampleAttributes().GetFields();
    for (const auto& tag : infoTag) {
      InfoField infoField(tag.attr, tag.attr_type, static_cast<uint8_t>(tag.attr_values.size()));
      core::record::sample::Info_tag infotag{
          static_cast<uint8_t>(tag.attr.size()), tag.attr, tag.attr_type,
          static_cast<uint8_t>(tag.attr_values.size()), tag.attr_values};
      infoTags[tag.attr] = infotag;
      attributeInfo[tag.attr] = infoField;
    }
  }
  readForTags.close();
  for (const auto& info : infoTags)
    infoFields.emplace_back(info.second.info_tag, info.second.info_type,
                            info.second.info_array_len);
}

SampleUnits SampleAnnotation::parseSample(std::ifstream& inputfile) {
  variant_sample::VariantSampleParser parser(inputfile, infoFields, defaultTileSizeWidth);
  uint8_t AG_class = 2;
  uint8_t AT_ID = 1;

  AnnotationEncoder encodingPars;
  entropy::bsc::BSCParameters bscParameters;
  auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

  encodingPars.setDescriptorParameters(core::AnnotDesc::LINKID, core::AlgoID::BSC,
                                       BSCalgorithmParameters);
  encodingPars.setCompressors(compressors);
  encodingPars.setAttributes(parser.getAttributes().getInfo());
  auto annotationEncodingParameters = encodingPars.Compose();
  ParameterSetComposer parameterset;

  annotationParameterSet = parameterset.Compose(AT_ID, AG_class, {defaultTileSizeWidth, 0},
                                                annotationEncodingParameters);

  variant_site::AccessUnitComposer accessUnit;
  accessUnit.setATtype(annotationType_,
                       annotationSubtype_);
  accessUnit.setCompressors(compressors);
  annotationAccessUnits.resize(parser.getNrOfTiles());
  uint64_t rowIndex = 0;

  std::map<std::string, core::access_unit::annotation::TypedData> attr;
  auto nrOfRemainingColumns = parser.getNumberOfColumns();
  for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
    std::map<core::AnnotDesc, std::stringstream> desc;
    for (auto& attrtile : parser.getAttributes().getTiles()) {
      attr[attrtile.first] = attrtile.second.getTypedTile(i);
    }

    // add LINK_ID default values
    std::cerr << " add link values... " << std::endl;
    auto nrOfLinkIds = std::min(nrOfRemainingColumns, static_cast<size_t>(defaultTileSizeWidth));
    for (auto j = 0u; j < nrOfLinkIds; ++j) {
        const char val = '\xFF';
        desc[core::AnnotDesc::LINKID].write(&val, 1);
        nrOfRemainingColumns--;
    }
    accessUnit.setAccessUnit(desc, attr, parser.getAttributes().getInfo(), annotationParameterSet,
                             annotationAccessUnits.at(i), AG_class, AT_ID, rowIndex);
    rowIndex++;
  }
  return SampleUnits{annotationParameterSet, annotationAccessUnits};
}

}  // namespace annotation
}  // namespace genie

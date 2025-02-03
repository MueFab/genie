/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <codecs/include/mpegg-codecs.h>
#include "annotation.h"
#include "genie/core/arrayType.h"
#include "genie/util/runtime_exception.h"

#include "contact_annotation.h"
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"

#include "AnnotationEncoder.h"
#include "ParameterSetComposer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

CMUnits CMAnnotation::parseContact(std::ifstream& inputfile) {
    uint8_t AG_class = 0;
    uint8_t AT_ID = 2;
    std::vector<genie::core::record::ContactRecord> RECS;
    genie::util::BitReader reader(inputfile);

    genie::util::BitReader bitreader(reader);

    while (bitreader.IsStreamGood()) {
        RECS.emplace_back(bitreader);
    }

    RECS.pop_back();
    auto cm_param = genie::contact::ContactMatrixParameters();
    auto scm_param = genie::contact::SubcontactMatrixParameters();
    auto scm_payload = genie::contact::SubcontactMatrixPayload();

    cm_param.setBinSize(RECS.front().getBinSize());
    cm_param.setTileSize(contactMatrixParameters.TILE_SIZE);

    for (auto& rec : RECS) {
        cm_param.upsertSample(rec.getSampleID(), rec.getSampleName());
        cm_param.upsertChromosome(rec.getChr1ID(), rec.getChr1Name(), rec.getChr1Length());
        cm_param.upsertChromosome(rec.getChr2ID(), rec.getChr2Name(), rec.getChr2Length());
    }

    auto& REC = RECS.front();
    auto rec = genie::core::record::ContactRecord(REC);
    genie::contact::encode_scm(cm_param, rec, scm_param, scm_payload, contactMatrixParameters.REMOVE_UNALIGNED_REGION,
                               contactMatrixParameters.TRANSFORM_MASK, contactMatrixParameters.ENA_DIAG_TRANSFORM,
                               contactMatrixParameters.ENA_BINARIZATION, contactMatrixParameters.NORM_AS_WEIGHT,
                               contactMatrixParameters.MULTIPLICATIVE_NORM, contactMatrixParameters.CODEC_ID);

    AnnotationEncoder encodingPars;
    encodingPars.setCompressors(compressors);
    encodingPars.setContactParameters(cm_param, {scm_param});

    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    encodingPars.setDescriptorParameters(genie::core::AnnotDesc::LINKID, genie::core::AlgoID::BSC,
                                         BSCalgorithmParameters);

    auto annotationEncodingParameters = encodingPars.Compose();
    ParameterSetComposer parameterset;

    CMUnits dataunits;
    dataunits.annotationParameterSet =
        parameterset.Compose(AT_ID, AG_class, {defaultTileSizeHeight, defaultTileSizeWidth}, annotationEncodingParameters);

    variant_site::AccessUnitComposer accessUnitcomposer;
    accessUnitcomposer.setCompressors(compressors);

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    std::map<std::string, core::record::annotation_parameter_set::AttributeData> attributeInfo;
    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    util::BitWriter writer(&descriptorStream[genie::core::AnnotDesc::CONTACT]);
    scm_payload.write(writer);
    core::record::annotation_access_unit::Record annotationAccessUnit;
    // add LINK_ID default values
    uint64_t linkIdRowCnt = 0;
    for (auto j = 0u; j < defaultTileSizeHeight && linkIdRowCnt < RECS.size(); ++j, ++linkIdRowCnt) {
        const char val = '\xFF';
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    accessUnitcomposer.setATtype(core::record::annotation_access_unit::AnnotationType::CONTACT_MATRICES, 10);
    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributeInfo,
                                     dataunits.annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);

    dataunits.annotationAccessUnit.push_back(annotationAccessUnit);

    return dataunits;
}

}  // namespace annotation
}  // namespace genie

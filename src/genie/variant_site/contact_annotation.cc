/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/variant_site/contact_annotation.h"

// Removed legacy codecs include

#include <map>
#include <string>
#include <vector>

#include "genie/variant_site/annotation.h"
#include "genie/core/array_type.h"
#include "genie/util/runtime_exception.h"

#include "genie/contact/contact_coder.h"
#include "genie/core/record/annotation/contact/record.h"

#include "genie/variant_site/annotation_encoder.h"

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

    cm_param.SetBinSize(RECS.front().GetBinSize());
    cm_param.SetTileSize(contactMatrixParameters.TILE_SIZE);

    for (auto& rec : RECS) {
        cm_param.UpsertSample(rec.GetSampleID(), rec.GetSampleName());
        cm_param.UpsertChromosome(rec.GetChr1ID(), rec.GetChr1Name(),
                                  rec.GetChr1Length());
        cm_param.UpsertChromosome(rec.GetChr2ID(), rec.GetChr2Name(),
                                  rec.GetChr2Length());
    }
    if (!contactMatrixParameters.bin_size_multipliers.empty()) {
        for (auto mult : contactMatrixParameters.bin_size_multipliers) {
            cm_param.UpsertBinSizeMultiplier(mult);
        }
    } else {
        cm_param.UpsertBinSizeMultiplier(contactMatrixParameters.MULT);
    }

    std::vector<genie::contact::SubcontactMatrixParameters> scm_params;
    std::vector<genie::contact::SubcontactMatrixPayload> scm_payloads;
    scm_params.reserve(RECS.size());
    scm_payloads.reserve(RECS.size());

    for (auto& REC : RECS) {
        auto rec = genie::core::record::ContactRecord(REC);
        genie::contact::SubcontactMatrixParameters scm_param;
        genie::contact::SubcontactMatrixPayload scm_payload;
        genie::contact::encode_scm(cm_param, rec, scm_param, scm_payload, contactMatrixParameters.REMOVE_UNALIGNED_REGION,
                                   contactMatrixParameters.TRANSFORM_MASK, contactMatrixParameters.ENA_DIAG_TRANSFORM,
                                   contactMatrixParameters.ENA_BINARIZATION, contactMatrixParameters.NORM_AS_WEIGHT,
                                   contactMatrixParameters.MULTIPLICATIVE_NORM, contactMatrixParameters.CODEC_ID);
        scm_params.push_back(std::move(scm_param));
        scm_payloads.push_back(std::move(scm_payload));
    }

    AnnotationEncoder encodingPars;
    encodingPars.setCompressors(compressors);
    encodingPars.setContactParameters(cm_param, scm_params);

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

    std::map<std::string, genie::core::access_unit::annotation::TypedData> attributeTDStream;
    std::map<std::string, core::parameter::annotation::AttributeData> attributeInfo;
    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    util::BitWriter writer(descriptorStream[genie::core::AnnotDesc::CONTACT]);
    for (auto& pay : scm_payloads) {
        pay.Write(writer);
    }
    core::access_unit::annotation::AccessUnit annotationAccessUnit;
    // add LINK_ID default values
    uint64_t linkIdRowCnt = 0;
    for (auto idx_j = 0u; idx_j < defaultTileSizeHeight && linkIdRowCnt < RECS.size(); ++idx_j, ++linkIdRowCnt) {
        const char val = '\xFF';
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    accessUnitcomposer.setATtype(core::access_unit::annotation::AnnotationType::CONTACT_MATRICES, 10);
    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributeInfo,
                                     dataunits.annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);

    dataunits.annotationAccessUnit.push_back(annotationAccessUnit);

    return dataunits;
}

}  // namespace annotation
}  // namespace genie

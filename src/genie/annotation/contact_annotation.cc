#include <codecs/include/mpegg-codecs.h>
#include "annotation.h"
#include "genie/core/arrayType.h"
#include "genie/util/runtime-exception.h"

#include "contact_annotation.h"
#include "genie/contact/contact_coder.h"
#include "genie/core/record/contact/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {


CMUnits CMAnnotation::parseContact(std::ifstream& inputfile) {

    uint8_t AT_ID = 2;
    std::vector<genie::core::record::ContactRecord> RECS;
    genie::util::BitReader reader(inputfile);

    genie::util::BitReader bitreader(reader);

    while (bitreader.isGood()) {
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



       genie::genotype::ParameterSetComposer parameterSetComposer;
    parameterSetComposer.setContactParameters(cm_param);
    parameterSetComposer.SetSubContactParameters(scm_param);
   parameterSetComposer.setCompressors(compressors);
    CMUnits dataunits; 
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> info;
    dataunits.annotationParameterSet = parameterSetComposer.Build(AT_ID,info,
                                                                  {defaultTileSizeHeight, defaultTileSizeWidth});


   /* auto obj_payload = std::stringstream();
    std::ostream& writer = obj_payload;
    auto bitwriter = genie::util::BitWriter(&writer);
    scm_payload.write(bitwriter);
    */
    return dataunits;
}

}  // namespace annotation
}  // namespace genie

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_CONTACT_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_CONTACT_ANNOTATION_H_

#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include "genie/core/constants.h"

#include "genie/contact/contact_coder.h"
#include "genie/core/variant_genotype_record/record.h"

#include "genie/annotation/compressors.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {

struct CMUnits {
    core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record> annotationAccessUnit;
};
struct ContactMatrixParameters {
    bool REMOVE_UNALIGNED_REGION{false};
    bool TRANSFORM_MASK{false};
    bool ENA_DIAG_TRANSFORM{true};
    bool ENA_BINARIZATION{true};
    bool NORM_AS_WEIGHT{true};
    bool MULTIPLICATIVE_NORM{true};
    genie::core::AlgoID CODEC_ID{genie::core::AlgoID::JBIG};
    uint32_t TILE_SIZE{150u};
    uint32_t MULT{1u};
};

// ---------------------------------------------------------------------------------------------------------------------

class CMAnnotation {
 public:
    void setContactOptions(ContactMatrixParameters opt) { contactMatrixParameters = opt; }
    void setTileSize(uint32_t _defaultTileSizeHeight, uint32_t _defaultTileSizeWidth) {
        defaultTileSizeHeight = _defaultTileSizeHeight;
        defaultTileSizeWidth = _defaultTileSizeWidth;
    }

    CMUnits parseContact(std::ifstream& inputfile);
    void setCompressors(genie::annotation::Compressor& _compressors) { compressors = _compressors; }
    void setCMparameters(ContactMatrixParameters cmParameters) { contactMatrixParameters = cmParameters; }

 private:
    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;
    ContactMatrixParameters contactMatrixParameters;
    genie::annotation::Compressor compressors;
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_CONTACT_ANNOTATION_H_

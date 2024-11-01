/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ANNOTATION_CONTACT_ANNOTATION_H
#define GENIE_ANNOTATION_CONTACT_ANNOTATION_H

#include <fstream>
#include <map>
#include <sstream>
#include "genie/core/constants.h"

#include "genie/contact/contact_coder.h"
#include "genie/core/record/variant_genotype/record.h"

#include "genie/annotation/Compressors.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

class GenotypeAnnotation {
 public:
    void setContactOptions(genie::likelihood::EncodingOptions opt) { (void) opt; }
    void setTileSize(uint32_t _defaultTileSizeHeight, uint32_t _defaultTileSizeWidth) {
        defaultTileSizeHeight = _defaultTileSizeHeight;
        defaultTileSizeWidth = _defaultTileSizeWidth;
    } 

    void parseContact(std::ifstream& inputfile);
    void setCompressors(genie::annotation::Compressor& _compressors) { compressors = _compressors; }

 private:
    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;

    genie::annotation::Compressor compressors;

};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_CONTACT_ANNOTATION_H

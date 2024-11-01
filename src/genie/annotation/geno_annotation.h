/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ANNOTATION_GENO_ANNOTATION_H
#define GENIE_ANNOTATION_GENO_ANNOTATION_H

#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include "genie/core/constants.h"

#include "genie/contact/contact_coder.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

#include "genie/annotation/Compressors.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

struct GenoUnits {
    core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record> annotationAccessUnit;
};

class GenoAnnotation {
 public:
    void setLikelihoodOptions(genie::likelihood::EncodingOptions opt) { likelihood_opt = opt; }
    void setGenotypeOptions(genie::genotype::EncodingOptions opt) { genotype_opt = opt; }
    void setTileSize(uint32_t _defaultTileSizeHeight, uint32_t _defaultTileSizeWidth) {
        defaultTileSizeHeight = _defaultTileSizeHeight;
        defaultTileSizeWidth = _defaultTileSizeWidth;
    } 

    GenoUnits parseGenotype(std::ifstream& inputfile);
    void setCompressors(genie::annotation::Compressor& _compressors) { compressors = _compressors; }

 private:
    typedef std::vector<genie::core::record::VariantGenotype> Tile;  // one tile contains a number of records

    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;

    genie::likelihood::EncodingOptions likelihood_opt{200, true};
    genie::genotype::EncodingOptions genotype_opt{200,                                          // block size
                                                  genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
                                                  genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
                                                  false,                                        // transpose_mat;
                                                  genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
                                                  genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
                                                  genie::core::AlgoID::JBIG};
    genie::annotation::Compressor compressors;

    struct RecData {
        uint32_t rowStart;
        uint32_t colStart;
        genie::genotype::EncodingBlock genotypeDatablock;
        genie::likelihood::EncodingBlock likelihoodDatablock;
        uint32_t numSamples;
        uint8_t formatCount;
        RecData(uint32_t _rowStart, uint32_t _colStart, genie::genotype::EncodingBlock _genotypeDatablock,
                genie::likelihood::EncodingBlock _likelihoodDatablock, uint32_t _numSamples, uint8_t _formatCount)
            : rowStart(_rowStart),
              colStart(_colStart),
              genotypeDatablock(_genotypeDatablock),
              likelihoodDatablock(_likelihoodDatablock),
              numSamples(_numSamples),
              formatCount(_formatCount) {}
    };
    size_t readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize, const uint32_t& colTilesize,
                      genie::genotype::GenotypeParameters& genotypeParameters,
                      genie::likelihood::LikelihoodParameters& likelihoodParameters, std::vector<RecData>& recData);

    std::vector<genie::core::record::VariantGenotype> splitOnRows(genie::core::record::VariantGenotype& rec,
                                                                  uint32_t colWidth);
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_GENO_ANNOTATION_H

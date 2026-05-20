/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_GENO_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_GENO_ANNOTATION_H_

#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/contact/contact_coder.h"
#include "genie/core/constants.h"
#include "genie/core/variant_genotype_record/record.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/annotation/compressors.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

struct GenoUnits {
    core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record>
        annotationAccessUnit;
};

class GenoAnnotation {
 public:
    void setLikelihoodOptions(likelihood::EncodingOptions opt) {
        likelihood_opt = opt;
    }
    void setGenotypeOptions(genotype::EncodingOptions opt) {
        genotype_opt = opt;
    }
    void setTileSize(uint32_t _defaultTileSizeHeight,
                     uint32_t _defaultTileSizeWidth) {
        defaultTileSizeHeight = _defaultTileSizeHeight;
        defaultTileSizeWidth = _defaultTileSizeWidth;
    }

    using AttrType = std::vector<uint8_t>;

    class RecData {
     public:
        uint32_t rowStart;
        uint32_t colStart;
        genotype::GenotypeParameters pars;
        genotype::GenotypePayload payload;
        /* std::tuple<genotype::GenotypeParameters,
                   genotype::GenotypePayload>
            genotypeData;*/
        std::map<std::string,
                std::tuple<core::record::annotation_parameter_set::AttributeData,
                    std::vector<std::vector<std::vector<AttrType>>>>>
            attributes;
        // genotype::EncodingBlock genotypeDatablock;
        likelihood::EncodingBlock likelihoodDatablock;
        uint32_t numSamples;
        uint8_t formatCount;

        RecData();
        RecData(
            uint32_t _rowStart, uint32_t _colStart,
            std::tuple<genotype::GenotypeParameters,
                       genotype::GenotypePayload>
                _genotypeData,
            likelihood::EncodingBlock _likelihoodDatablock,
            uint32_t _numSamples, uint8_t _formatCount,
            std::map<
                std::string,
                std::tuple<core::record::annotation_parameter_set::AttributeData,
                           std::vector<std::vector<std::vector<AttrType>>>>>
                attributes);

        RecData& operator=(const RecData& other);

        void set(
            uint32_t _rowStart, uint32_t _colStart,
            std::tuple<genotype::GenotypeParameters,
                    genotype::GenotypePayload>
                _genotypeData,
            likelihood::EncodingBlock _likelihoodDatablock,
            uint32_t _numSamples, uint8_t _formatCount,
            std::map<
                std::string,
                std::tuple<core::record::annotation_parameter_set::AttributeData,
                        std::vector<std::vector<std::vector<AttrType>>>>>
                _attributes);
    };

    // std::vector<GenoUnits> parseGenotype(std::ifstream& inputfile);
    std::vector<GenoUnits> parseGenotype(
        std::ifstream& inputfile,
        std::vector<std::pair<uint64_t, uint8_t>>& numBitPlanes);
    void setCompressors(Compressor& _compressors) {
        compressors = _compressors;
    }

 private:
    typedef std::vector<core::record::VariantGenotype>
        Tile;  // one tile contains a number of records

    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;

    likelihood::EncodingOptions likelihood_opt{200, true};
    genotype::EncodingOptions genotype_opt{
        200,                                   // block size
        genotype::BinarizationID::BIT_PLANE,   // binarization_ID_;
        genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis_;
        false,                                 // transpose_mat;
        genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
        genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
        core::AlgoID::JBIG};
    Compressor compressors;

    std::map<std::string, core::record::annotation_parameter_set::AttributeData>
        attrInfo;
    std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>>
        attrValues;

    void sort_format(
        std::vector<core::record::VariantGenotype>& varGenoType);

    struct ParsBlocks {
        genotype::GenotypeParameters genotypePars;
        likelihood::LikelihoodParameters likelihoodPars;
        std::vector<RecData> blocks;
        uint32_t rows;
    };

    size_t readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize,
                        std::vector<ParsBlocks>& blocksWPars);

    size_t readOneBlock(
        util::BitReader& reader, const uint32_t& rowTileSize,
        genotype::GenotypeParameters& genotypeParameters,
        likelihood::LikelihoodParameters& likelihoodParameters,
        RecData& recData);
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_GENO_ANNOTATION_H_

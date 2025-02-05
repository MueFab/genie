/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "VariantSiteParser.h"

#include <algorithm>
#include <string>

#include "genie/core/record/annotation_access_unit/AnnotationAccessUnitHeader.h"
#include "genie/core/record/annotation_access_unit/block.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/core/record/variant_genotype/record.h"
#include "genie/core/record/variant_site/record.h"

#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {
void AccessUnitComposer::setAccessUnit(
    std::map<core::AnnotDesc, std::stringstream>& _descriptorStream,
    std::map<std::string, core::record::annotation_access_unit::TypedData>& _attributeTileStream,
    std::map<std::string, core::record::annotation_parameter_set::AttributeData> _attributeInfo,
    const core::record::annotation_parameter_set::Record& _annotationParameterSet,
    core::record::annotation_access_unit::Record& _annotationAccessUnit, uint8_t _AG_class, uint8_t _AT_ID,
    uint64_t _rowIndex) {
    setAccessUnit(_descriptorStream, _attributeTileStream, _attributeInfo, _annotationParameterSet,
                  _annotationAccessUnit, _AG_class, _AT_ID, _rowIndex, 0);
}
void AccessUnitComposer::setAccessUnit(
    std::map<core::AnnotDesc, std::stringstream>& _descriptorStream,
    std::map<std::string, core::record::annotation_access_unit::TypedData>& _attributeTileStream,
    std::map<std::string, core::record::annotation_parameter_set::AttributeData> _attributeInfo,
    const core::record::annotation_parameter_set::Record& _annotationParameterSet,
    core::record::annotation_access_unit::Record& _annotationAccessUnit, uint8_t _AG_class, uint8_t _AT_ID,
    uint64_t _rowIndex, uint64_t _colIndex) {
    tile_index_2 = _colIndex;
    tile_index_1 = _rowIndex;
    std::vector<genie::core::record::annotation_access_unit::Block> blocks;
    AT_ID = _AT_ID;
    AG_class = _AG_class;
    if (AG_class == 0 || AG_class == 2) //genotype or contact
        tile_index_2_exists = true;
    else
        tile_index_2_exists = false;

    // -------- descriptors ---------- //
    auto descriptorConfigurations =
        _annotationParameterSet.getAnnotationEncodingParameters().getDescriptorConfigurations();
    std::map<core::AnnotDesc, std::stringstream> encodedDescriptors;
    compress(descriptorConfigurations, _descriptorStream, encodedDescriptors);

    for (auto it = encodedDescriptors.begin(); it != encodedDescriptors.end(); ++it) {
        std::vector<uint8_t> data;
        for (auto readbyte : it->second.str()) data.push_back(readbyte);
        genie::core::record::annotation_access_unit::BlockVectorData blockInfo(it->first, data);
        genie::core::record::annotation_access_unit::Block block;
        if (data.size() != 0) {
            block.set(blockInfo);
            blocks.push_back(block);
        }
    }

    // -------- attributes ----------- //
    auto attributeParameterSets = _annotationParameterSet.getAnnotationEncodingParameters().getAttributeParameterSets();
    auto compressorParameterSets =
        _annotationParameterSet.getAnnotationEncodingParameters().getCompressorParameterSets();
    std::map<std::string, std::stringstream> encodedAttributes;

    // auto temp1 = _attributeTileStream["AA"].getdata().str().length();

    for (auto& attribute : attributeParameterSets) {
        auto compressorId = attribute.getCompressorID();
        if (compressorId != 0) {
            auto& AttributeStream = _attributeTileStream[attribute.getAttributeName()];
            auto& compressorSet = compressorParameterSets.at(compressorId - 1);
            compress(AttributeStream, compressorSet);
        }
    }

    for (auto& tile : _attributeTileStream) {
        std::stringstream data;
        genie::core::Writer writer(&data);
        tile.second.write(writer);
        writer.Flush();
        auto attributeID = _attributeInfo[tile.first].getAttributeID();
        core::record::annotation_access_unit::BlockData blockInfo(core::AnnotDesc::ATTRIBUTE, attributeID, data);
        genie::core::record::annotation_access_unit::Block block;
        block.set(blockInfo);
        blocks.push_back(block);
    }

    // ------------------------------- //

    uint64_t n_blocks = static_cast<uint64_t>(blocks.size());
    ATCoordSize = _annotationParameterSet.getATCoordSize();
    genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader annotationAccessUnitHeadertot(
        attributeContiguity, twoDimensional, columnMajorTileOrder, variable_size_tiles, ATCoordSize, is_attribute,
        attribute_ID, descriptor_ID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1, tile_index_2_exists,
        tile_index_2);
    genie::core::record::annotation_access_unit::Record annotationAccessUnit(
        AT_ID, AT_type, AT_subtype, AG_class, annotationAccessUnitHeadertot, blocks, attributeContiguity,
        twoDimensional, columnMajorTileOrder, ATCoordSize, variable_size_tiles, n_blocks, numChrs);
    _annotationAccessUnit = annotationAccessUnit;
}

void AccessUnitComposer::compress(
    const std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration>& descriptorConfigurations,
    std::map<genie::core::AnnotDesc, std::stringstream>& inputstream,
    std::map<genie::core::AnnotDesc, std::stringstream>& encodedDescriptors) {
    genie::entropy::bsc::BSCEncoder bscEncoder;
    genie::entropy::lzma::LZMAEncoder lzmaEncoder;
    genie::entropy::zstd::ZSTDEncoder zstdEncoder;
    for (auto encodingpar : descriptorConfigurations) {
        if (encodingpar.getDescriptorID() == genie::core::AnnotDesc::GENOTYPE ||
            encodingpar.getDescriptorID() == genie::core::AnnotDesc::LIKELIHOOD ||
            encodingpar.getDescriptorID() == genie::core::AnnotDesc::CONTACT)
            encodedDescriptors[encodingpar.getDescriptorID()] << inputstream[encodingpar.getDescriptorID()].rdbuf();
        else
            switch (encodingpar.getEncodingModeID()) {
                case genie::core::AlgoID::BSC:
                    bscEncoder.encode(inputstream[encodingpar.getDescriptorID()],
                                      encodedDescriptors[encodingpar.getDescriptorID()]);
                    break;
                case genie::core::AlgoID::LZMA:
                    lzmaEncoder.encode(inputstream[encodingpar.getDescriptorID()],
                                       encodedDescriptors[encodingpar.getDescriptorID()]);
                    break;
                case genie::core::AlgoID::ZSTD:
                    zstdEncoder.encode(inputstream[encodingpar.getDescriptorID()],
                                       encodedDescriptors[encodingpar.getDescriptorID()]);
                    break;

                default:
                    break;
            }
    }
}

void AccessUnitComposer::compress(
    std::map<std::string, std::stringstream>& attributeStream,
    const std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet>& attributeParameterSets,
    const std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet>& compressorParameterSets,
    std::map<std::string, std::stringstream>& encodedAttributes) {
    genie::entropy::bsc::BSCEncoder bscEncoder;
    genie::entropy::lzma::LZMAEncoder lzmaEncoder;
    genie::entropy::zstd::ZSTDEncoder zstdEncoder;

    for (auto& attribute : attributeParameterSets) {
        auto attributeName = attribute.getAttributeName();
        auto compressorID = attribute.getCompressorID();
        if (compressorID == 0) {
            encodedAttributes[attributeName] << attributeStream[attributeName].rdbuf();
        } else {
            auto encodeID = compressorParameterSets[compressorID - 1].getAlgorithmIDs();

            switch (encodeID[0]) {
                case genie::core::AlgoID::BSC:
                    bscEncoder.encode(attributeStream[attributeName], encodedAttributes[attributeName]);
                    break;
                case genie::core::AlgoID::LZMA:
                    lzmaEncoder.encode(attributeStream[attributeName], encodedAttributes[attributeName]);
                    break;
                case genie::core::AlgoID::ZSTD:
                    zstdEncoder.encode(attributeStream[attributeName], encodedAttributes[attributeName]);
                    break;

                default:
                    encodedAttributes[attributeName] << attributeStream[attributeName].rdbuf();
                    break;
            }
        }
    }
}

void AccessUnitComposer::compress(genie::core::record::annotation_access_unit::TypedData& oneBlock,
                                  genie::core::record::annotation_parameter_set::CompressorParameterSet& compressor) {
    auto encodeId = compressor.getAlgorithmIDs();
    if (compressor.getCompressorID() != 0) {
        std::stringstream compressedData;
        compressors.compress(oneBlock.getdata(), compressedData, compressor.getCompressorID());
        oneBlock.setCompressedData(compressedData);
    }
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <tuple>
#include <utility>

#include "apps/genie/annotation/code.h"
#include "filesystem/filesystem.hpp"
#include "genie/core/writer.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/string_helpers.h"

#include "genie/core/record/data_unit/record.h"
#include "genie/variantsite/accessunit_composer.h"
#include "genie/variantsite/parameterset_composer.h"
#include "genie/variantsite/variantsite_parser.h"

#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/parameterset_composer.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/likelihood/likelihood_payload.h"
// ---------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#define SYSERROR() GetLastError()
#else
#include <errno.h>
#define SYSERROR() errno
#endif

namespace genieapp {
namespace annotation {

// ---------------------------------------------------------------------------------------------------------------------

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName)
    : Code(_inputFileName, _outputFileName, false, "", "") {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName,
           const std::string& _infoFieldsFileName)
    : Code(_inputFileName, _outputFileName, false, _infoFieldsFileName, "") {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput)
    : Code(_inputFileName, _outputFileName, testOutput, "", "") {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
           const std::string& rec)
    : Code(_inputFileName, _outputFileName, testOutput, "", rec) {}

Code::Code(const std::string& inputFileName, const std::string& outputFileName, bool testOutput,
           const std::string& infoFieldsFileName, const std::string& _rec) {
    if (inputFileName.empty()) {
        std::cerr << ("No Valid Inputs ") << std::endl;
        return;
    }
    auto rec = _rec;
    std::transform(rec.begin(), rec.end(), rec.begin(), [](unsigned char c) { return std::tolower(c); });
    std::cerr << rec << ": ";
    if (rec == "geno") {
        std::cerr << "encoding variant genotype... \n";
        std::string input_fpath = "datain";
        encodeVariantGenotype(inputFileName, outputFileName);
    } else {
        std::cerr << "encoding varaint site... \n";
        encodeVariantSite(inputFileName, outputFileName, testOutput, infoFieldsFileName);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encodeVariantSite(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
                       const std::string& _infoFieldsFileName) {
    //------------- read input files  ------------------//
    std::stringstream infoFields;
    uint8_t AT_ID = 1;
    uint8_t AG_class = 1;

    auto infoFieldsFileName = _infoFieldsFileName;
    if (!infoFieldsFileName.empty()) {
        std::cerr << "reading infofields from " << infoFieldsFileName << std::endl;
        std::ifstream infoFieldsFile;
        infoFieldsFile.open(infoFieldsFileName, std::ios::in);
        if (infoFieldsFile.is_open()) {
            infoFields << infoFieldsFile.rdbuf();
            infoFieldsFile.close();
        }
    }
    std::ifstream site_MGrecs;
    auto inputFileName = _inputFileName;
    site_MGrecs.open(inputFileName, std::ios::in | std::ios::binary);
    //------------- -------------  ------------------//

    genie::variant_site::VariantSiteParser parser(site_MGrecs, infoFields, 0);

    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    auto info = parser.getAttributes().getInfo();

    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    genie::variant_site::ParameterSetComposer encodeParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descrList, info, parser.getNumberOfRows(), AT_ID);

    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit(parser.getNrOfTiles());

    genie::variant_site::AccessUnitComposer accessUnit;
   // uint8_t AT_ID = 1;

    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<genie::core::AnnotDesc, std::stringstream> desc;
        for (auto& desctile : tile_descriptorStream) {
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();
        }

        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attr;
        for (auto& attrtile : tile_attributeStream) {
            attr[attrtile.first] = attrtile.second.getTypedTile(i);
        }
        accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit.at(i), AG_class, AT_ID,
                                 (uint8_t)0);
    }

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    std::ofstream txtFile;
    if (testOutput) {
        std::string txtName = _outputFileName + ".txt";
        txtFile.open(txtName, std::ios::out);
    }

    std::ofstream outputFile;
    outputFile.open(_outputFileName, std::ios::binary | std::ios::out);

    if (outputFile.is_open()) {
        genie::core::Writer dataUnitWriter(&outputFile);
        APS_dataUnit.write(dataUnitWriter);
        for (auto& aau : annotationAccessUnit) {
            genie::core::record::data_unit::Record AAU_dataUnit(aau);
            AAU_dataUnit.write(dataUnitWriter);
        }
        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.GetBitsWritten() / 8) << std::endl;
        outputFile.close();
        if (testOutput) {
            genie::core::Writer txtWriter(&txtFile, true);
            APS_dataUnit.write(txtWriter);
            for (auto& aau : annotationAccessUnit) {
                genie::core::record::data_unit::Record AAU_dataUnit(aau);
                AAU_dataUnit.write(txtWriter);
            }
            txtWriter.Flush();
            txtFile.close();
        }
    } else {
        std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
    }
}

void encodeVariantGenotype(const std::string& _input_fpath, const std::string& _output_fpath) {
    std::ifstream reader(_input_fpath, std::ios::binary);
    genie::util::BitReader bitreader(reader);
    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO(Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    uint32_t BLOCK_SIZE = 100;
    bool TRANSFORM_MODE = true;

    genie::likelihood::EncodingOptions likelihood_opt = {
        BLOCK_SIZE,      // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    genie::genotype::EncodingOptions genotype_opt = {
        BLOCK_SIZE,                                  // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID_;
    };

    auto genotypeData = genie::genotype::encode_block(genotype_opt, recs);
    auto likelihoodData = genie::likelihood::encode_block(likelihood_opt, recs);

    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters =
        std::get<genie::genotype::GenotypeParameters>(genotypeData);
    auto datablock = std::get<genie::genotype::EncodingBlock>(genotypeData);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genotypeParameterSet.setGenotypeParameters(genotypeParameters);
    genotypeParameterSet.setLikelihoodParameters(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData));
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, {recs.size(), 3000});

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    for (auto formatdata : datablock.attributeData) {
        auto& info = attributesInfo[formatdata.first];
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(std::min(BLOCK_SIZE, static_cast<uint32_t>(recs.size())));
        arrayDims.push_back(recs.at(0).getNumSamples());
        arrayDims.push_back(recs.at(0).getFormatCount());
        attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                arrayDims);
        attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }

    descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
    {
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }

    // add LINK_ID default values
    /* for (auto i = 0u; i < BLOCK_SIZE && i < recs.size(); ++i) {
        const char val = '\xFF';
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }*/
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    // ---------------------------------------- //

    std::ofstream outputFile;
    outputFile.open(_output_fpath, std::ios::binary | std::ios::out);

    if (outputFile.is_open()) {
        genie::core::Writer dataUnitWriter(&outputFile);
        APS_dataUnit.write(dataUnitWriter);
        AAU_dataUnit.write(dataUnitWriter);

        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.GetBitsWritten() / 8) << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
    }
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

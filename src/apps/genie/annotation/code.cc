/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <utility>

#include "apps/genie/annotation/code.h"
//#include "codecs/api/mpegg_utils.h"
//#include "codecs/include/mpegg-codecs.h"
#include "filesystem/filesystem.hpp"
#include "genie/core/writer.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"

#include "genie/core/record/data_unit/record.h"
#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"
#include "genie/variantsite/VariantSiteParser.h"

#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"

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
    std::stringstream infoFields;
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

    uint8_t AT_ID = 1;
    genie::variant_site::VariantSiteParser parser(site_MGrecs, infoFields, 0);

    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    auto info = parser.getAttributes().getInfo();
    std::map<genie::core::AnnotDesc, std::stringstream> desc;
    std::map<std::string, std::stringstream> attr;

    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        for (auto& desctile : tile_descriptorStream) {
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();
        }

        for (auto& attrtile : tile_attributeStream) {
            attr[attrtile.first] << attrtile.second.getTile(i).rdbuf();
        }
    }

    genie::variant_site::ParameterSetComposer encodeParameters;

    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descrList, info, parser.getNumberOfRows());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    genie::variant_site::AccessUnitComposer accessUnit;
    uint8_t AG_class = 1;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit, AG_class, AT_ID);

    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::ofstream txtFile;
    if (testOutput) {
        std::string txtName = _outputFileName + ".txt";
        txtFile.open(txtName, std::ios::out);
    }

    std::cerr << "annotationAccessUnit size:" << annotationAccessUnit.getSize();
    std::cerr << "\nannotationParameterSet size:" << annotationParameterSet.getSize() << std::endl;

    std::ofstream outputFile;
    outputFile.open(_outputFileName, std::ios::binary | std::ios::out);

    if (outputFile.is_open()) {
        genie::core::Writer dataUnitWriter(&outputFile);
        APS_dataUnit.write(dataUnitWriter);
        AAU_dataUnit.write(dataUnitWriter);

        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.getBitsWritten() / 8) << std::endl;
        outputFile.close();
        if (testOutput) {
            genie::core::Writer txtWriter(&txtFile, true);
            APS_dataUnit.write(txtWriter);
            AAU_dataUnit.write(txtWriter);
            txtWriter.flush();
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

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    auto tupleoutput = genie::genotype::encode_block(opt, recs);
    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters = std::get<genie::genotype::GenotypeParameters>(tupleoutput);
    auto datablock = std::get<genie::genotype::EncodingBlock>(tupleoutput);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters, recs.size());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------
    genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;
    std::map<std::string, std::stringstream> attributeStream;
    for (auto formatdata : datablock.attributeData)
        for (size_t i = 0; i < formatdata.second.size(); ++i)
            attributeStream[formatdata.first].write((char*)&formatdata.second.at(i), 1);

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::ofstream outputFile;
    outputFile.open(_output_fpath, std::ios::binary | std::ios::out);

    if (outputFile.is_open()) {
        genie::core::Writer dataUnitWriter(&outputFile);
        APS_dataUnit.write(dataUnitWriter);
        AAU_dataUnit.write(dataUnitWriter);

        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.getBitsWritten() / 8) << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
    }
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

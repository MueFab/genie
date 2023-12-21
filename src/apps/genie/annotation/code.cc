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
        encodeParameters.setParameterSet(descrList, info, parser.getNumberOfRows());

    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit(parser.getNrOfTiles());

    genie::variant_site::AccessUnitComposer accessUnit;
    uint8_t AG_class = 1;
    uint8_t AT_ID = 0;

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
        std::cerr << "bytes written: " << std::to_string(dataUnitWriter.getBitsWritten() / 8) << std::endl;
        outputFile.close();
        if (testOutput) {
            genie::core::Writer txtWriter(&txtFile, true);
            APS_dataUnit.write(txtWriter);
            for (auto& aau : annotationAccessUnit) {
                genie::core::record::data_unit::Record AAU_dataUnit(aau);
                AAU_dataUnit.write(txtWriter);
            }
            txtWriter.flush();
            txtFile.close();
        }
    } else {
        std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
    }
}

std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    genie::genotype::EncodingOptions opt = {
        blockSize,                                   // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    return genie::genotype::encode_block(opt, recs);
}

std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    bool TRANSFORM_MODE = true;
    genie::likelihood::EncodingOptions opt = {
        blockSize,       // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    genie::likelihood::EncodingBlock block;
    genie::likelihood::extract_likelihoods(opt, block, recs);
    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;
    genie::likelihood::serialize_mat(block.idx_mat, block.dtype_id, block.nrows, block.ncols, block.serialized_mat);
    genie::likelihood::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    block.serialized_mat.seekp(0, std::ios::end);
    genie::likelihood::LikelihoodParameters parameters(static_cast<uint8_t>(recs.at(0).getNumberOfLikelihoods()),
                                                       TRANSFORM_MODE, block.dtype_id);
    return std::make_tuple(parameters, block);
}

void encodeVariantGenotype(const std::string& _input_fpath, const std::string& _output_fpath) {
    std::ifstream reader(_input_fpath, std::ios::binary);
    genie::util::BitReader bitreader(reader);
    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO(Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    uint32_t BLOCK_SIZE = 100;

    auto tupleoutput = genotypeEncoding(BLOCK_SIZE, recs);
    auto likelihoodblock = likelihoodEncoding(BLOCK_SIZE, recs);

    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters = std::get<genie::genotype::GenotypeParameters>(tupleoutput);
    auto datablock = std::get<genie::genotype::EncodingBlock>(tupleoutput);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters,
                                   std::get<genie::likelihood::LikelihoodParameters>(likelihoodblock), recs.size());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;
   /* std::map<std::string, std::stringstream> attributeStream;
    for (auto formatdata : datablock.attributeData)
        for (size_t i = 0; i < formatdata.second.size(); ++i)
            attributeStream[formatdata.first].write((char*)&formatdata.second.at(i), 1);*/

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    for (auto formatdata : datablock.attributeData) {
        auto& info = attributesInfo[formatdata.first];
        auto arraylength = info.getArrayLength();
        std::vector<uint32_t> arrayDims; arrayDims.push_back(BLOCK_SIZE);
        for (auto i = 1; i < arraylength; ++i) arrayDims.push_back(2);
        attributeTDStream[formatdata.first].set(info.getAttributeType(), info.getArrayLength(), arrayDims);
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
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodblock),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodblock));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID,0);
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

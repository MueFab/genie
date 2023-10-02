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

//#include "genie/entropy/bsc/encoder.h"
#include "genie/core/record/variant_genotype/record.h"
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

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
           const std::string& _infoFieldsFileName, const std::string& _rec)
    : inputFileName(_inputFileName),
      outputFileName(_outputFileName),
      infoFieldsFileName(_infoFieldsFileName),
      rec(_rec) {
    (void)rec;

    if (inputFileName.empty()) {
        std::cerr << ("No Valid Inputs ") << std::endl;
        return;
    }
    std::transform(rec.begin(), rec.end(), rec.begin(), [](unsigned char c) { return std::tolower(c); });
    std::cerr << _rec << ": ";
    if (rec == "geno") {
        std::cerr << "encoding variant genotype... \n";
        std::string input_fpath = "datain";
        encodeVariantGenotype(_inputFileName, _outputFileName);
    } else {
        std::cerr << "encoding varaint site... \n";
        encodeVariantSite(inputFileName, outputFileName, testOutput, infoFieldsFileName);
    }
}

genie::core::AlgoID Code::convertStringToALgoID(std::string algoString) const {
    const std::map<std::string, genie::core::AlgoID> algoIds = {{"GABAC", genie::core::AlgoID::CABAC},
                                                                {"LZMA", genie::core::AlgoID::LZMA},
                                                                {"ZSTD", genie::core::AlgoID::ZSTD},
                                                                {"BSC", genie::core::AlgoID::BSC},
                                                                {"PROCRUSTES", genie::core::AlgoID::PROCRUSTES},
                                                                {"JBIG", genie::core::AlgoID::JBIG},
                                                                {"LZW", genie::core::AlgoID::LZW},
                                                                {"BIN", genie::core::AlgoID::BIN},
                                                                {"SPARSE", genie::core::AlgoID::SPARSE},
                                                                {"DEL", genie::core::AlgoID::DEL},
                                                                {"RLE", genie::core::AlgoID::RLE},
                                                                {"SER", genie::core::AlgoID::SER}};
    if (algoIds.find(algoString) == algoIds.end()) return genie::core::AlgoID::BSC;
    return algoIds.at(algoString);
}

// ---------------------------------------------------------------------------------------------------------------------

void encodeVariantSite(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput,
                       const std::string& _infoFieldsFileName) {
    std::ifstream site_MGrecs;
    site_MGrecs.open(_inputFileName, std::ios::in | std::ios::binary);

    std::stringstream infoFields;
    if (!_infoFieldsFileName.empty()) {
        std::cerr << "reading infofields from " << _infoFieldsFileName << std::endl;
        std::ifstream infoFieldsFile;
        infoFieldsFile.open(_infoFieldsFileName, std::ios::in);
        if (infoFieldsFile.is_open()) {
            infoFields << infoFieldsFile.rdbuf();
            infoFieldsFile.close();
        }
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    std::map<std::string, genie::core::record::variant_site::AttributeData> attributesInfo;
    std::map<std::string, std::stringstream> attributeStream;
    genie::variant_site::VariantSiteParser parser(site_MGrecs, descriptorStream, attributesInfo, attributeStream,
                                                  infoFields);

    genie::variant_site::ParameterSetComposer encodeParameters(descriptorStream);

    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(attributesInfo, parser.getNumberOfRows());

    genie::variant_site::AccessUnitComposer accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    accessUnit.setAccessUnit(descriptorStream, attributeStream, attributesInfo, annotationParameterSet,
                             annotationAccessUnit);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
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
    (void)_output_fpath;

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
    genie::genotype::decompose(opt, block, recs);

    //--------------------------------------------------
    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    std::map<std::string, genie::core::record::variant_site::AttributeData> attributesInfo;
    std::map<std::string, std::stringstream> attributeStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    for (auto j : block.allele_mat) {
        descriptorStream[genie::core::AnnotDesc::GENOTYPE].write((const char*)j,1);
    }
    //block.phasing_mat;
    genie::variant_site::AccessUnitComposer accessUnit;
    genie::variant_site::ParameterSetComposer encodeParameters(descriptorStream);
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet( attributesInfo, recs.size());
    // auto& allele_mat = block.allele_mat;
    // auto& phasing_mat = block.phasing_mat;
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

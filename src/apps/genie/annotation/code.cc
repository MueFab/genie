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
    : Code(_inputFileName, _outputFileName, genie::core::AlgoID::BSC, false) {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName,
           const std::string& _infoFieldsFileName)
    : Code(_inputFileName, _outputFileName, genie::core::AlgoID::BSC, false, _infoFieldsFileName, "") {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, bool testOutput)
    : Code(_inputFileName, _outputFileName, genie::core::AlgoID::BSC, testOutput) {
}  // TODO @Stefanie: change default AlgoID

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, std::string encodeString,
           bool testOutput)
    : Code(_inputFileName, _outputFileName, convertStringToALgoID(encodeString), testOutput) {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
           bool testOutput, const std::string& rec)
    : Code(_inputFileName, _outputFileName, encodeMode, testOutput, "", rec) {}

Code::Code(const std::string& _inputFileName, const std::string& _outputFileName, genie::core::AlgoID encodeMode,
           bool testOutput, const std::string& _infoFieldsFileName, const std::string& rec)
    : inputFileName(_inputFileName),
      outputFileName(_outputFileName),
      infoFieldsFileName(_infoFieldsFileName),
      compressedData{} {
    rec;
    encodeMode;
    if (inputFileName.empty()) {
        std::cerr << ("No Valid Inputs ") << std::endl;
        return;
    }

    std::ifstream site_MGrecs;
    site_MGrecs.open(inputFileName, std::ios::in | std::ios::binary);

    

    std::stringstream infoFields;
    if (!infoFieldsFileName.empty()) {
        std::cerr << "reading infofields from " << infoFieldsFileName << std::endl;
        std::ifstream infoFieldsFile;
        infoFieldsFile.open(infoFieldsFileName, std::ios::in);
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


    genie::variant_site::ParameterSetComposer encodeParameters;

    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descriptorStream, attributesInfo, parser.getNumberOfRows());


    genie::variant_site::AccessUnitComposer accessUnit;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;
    accessUnit.setAccessUnit(descriptorStream, attributeStream, attributesInfo, annotationParameterSet,
                             annotationAccessUnit);

   genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    std::ofstream txtFile;
    if (testOutput) {
        std::string txtName = outputFileName + ".txt";
        txtFile.open(txtName, std::ios::out);
    }

    std::cerr << "annotationAccessUnit size:"<< annotationAccessUnit.getSize();
    std::cerr << "\nannotationParameterSet size:" << annotationParameterSet.getSize() << std::endl;

    std::ofstream outputFile;
    outputFile.open(outputFileName, std::ios::binary | std::ios::out);

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

void encodeVariantGenotype(std::string& _input_fpath, std::string& _output_fpath) {
    if (_input_fpath == _output_fpath) {
    }
}

}  // namespace annotation
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

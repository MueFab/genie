/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <chrono>

#include <codecs/include/mpegg-codecs.h>
#include "annotation.h"
#include "genie/core/arrayType.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void genie::annotation::Annotation::startStream(RecType recType, std::string recordInputFileName,
                                                std::string outputFileName)

{
    std::ifstream inputfile;
    inputfile.open(recordInputFileName, std::ios::in | std::ios::binary);

    if (!inputfile.is_open())  // failed
        return;

    // read file - determine type

    if (recType == RecType::GENO_FILE) {
        std::vector<std::pair<uint64_t, uint8_t>> numBitPlanes;
        std::string num_bit_plane_filename =  outputFileName+"_numBitPlane";
        if (std::filesystem::exists(num_bit_plane_filename)) std::filesystem::remove(num_bit_plane_filename);
        genoAnnotation.setCompressors(compressors);
        genoAnnotation.setTileSize(defaultTileSizeHeight, defaultTileSizeWidth);
        auto dataunits = genoAnnotation.parseGenotype(inputfile, numBitPlanes);

        for (auto& dataunit : dataunits) {
            annotationParameterSet.push_back(dataunit.annotationParameterSet);
            annotationAccessUnit.insert(annotationAccessUnit.end(), dataunit.annotationAccessUnit.begin(),
                                        dataunit.annotationAccessUnit.end());
        }
    } else if (recType == RecType::SITE_FILE) {
        siteAnnotation.setCompressors(compressors);
        siteAnnotation.parseInfoTags(recordInputFileName);
        auto dataunits = siteAnnotation.parseSite(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    } else {  // contact matrix
        cmAnnotation.setCompressors(compressors);
        cmAnnotation.setTileSize(defaultTileSizeHeight, defaultTileSizeWidth);
        auto dataunits = cmAnnotation.parseContact(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    }
    if (inputfile.is_open()) inputfile.close();

    writeToFile(outputFileName);
}

void Annotation::writeToFile(std::string& outputFileName) {
  std::cerr << " start of writeToFile... " << std::endl;
    std::ofstream testfile;
    std::string filename = outputFileName;
    testfile.open(filename + ".bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filename + ".txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);
    uint64_t sizeSofar = 0;

    for (auto& pars : annotationParameterSet) {
        genie::core::record::data_unit::Record APS_dataUnit(pars);
        sizeSofar = APS_dataUnit.write(testwriter);
        APS_dataUnit.write(txtwriter, sizeSofar);
    }
    for (auto& aau : annotationAccessUnit) {
      genie::core::record::data_unit::Record AAU_dataUnit(aau);
        sizeSofar = AAU_dataUnit.write(testwriter);
        AAU_dataUnit.write(txtwriter, sizeSofar);
    }
    testfile.close();
    txtfile.close();
}

}  // namespace annotation
}  // namespace genie

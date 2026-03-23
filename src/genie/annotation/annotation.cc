/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/util/runtime_exception.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void Annotation::startStream(RecType recType, std::string recordInputFileName,
                             std::string outputFileName) {
    std::ifstream inputfile;
    inputfile.open(recordInputFileName, std::ios::in | std::ios::binary);

    if (!inputfile.is_open())  // failed
        return;

    // read file - determine type

    if (recType == RecType::GENO_FILE) {
        std::vector<std::pair<uint64_t, uint8_t>> numBitPlanes;
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
    } else if (recType == RecType::SAMPLE_FILE) {
        sampleAnnotation.setCompressors(compressors);
        sampleAnnotation.setTileSize(defaultTileSizeWidth);
        sampleAnnotation.parseInfoTags(recordInputFileName);
        sampleAnnotation.setTileSize(defaultTileSizeWidth);
        sampleAnnotation.setATtype(annotationType_, annotationSubtype_);
        annotation::SampleUnits dataunits = sampleAnnotation.parseSample(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    } else if (recType == RecType::GENE_EXPRESSION_FILE) {  // gene expression
        geneExpressionAnnotation.setCompressors(compressors);
        geneExpressionAnnotation.setTileSize(defaultTileSizeHeight, defaultTileSizeWidth);
        auto dataunits = geneExpressionAnnotation.parseGeneExpression(inputfile);
        for (auto& dataunit : dataunits) {
          annotationParameterSet.push_back(dataunit.annotationParameterSet);
          annotationAccessUnit.insert(annotationAccessUnit.end(),
                                      dataunit.annotationAccessUnit.begin(),
                                      dataunit.annotationAccessUnit.end());
        }
    } else if (recType == RecType::FEATURE_FILE) {
        featureAnnotation.setCompressors(compressors);
        featureAnnotation.parseInfoTags(recordInputFileName);
        auto dataunits = featureAnnotation.parseFeature(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    } else if (recType == RecType::FUNCTIONAL_ANNOTATIONS_FILE) {
        functionalAnnotation.setCompressors(compressors);
        functionalAnnotation.setTileSize(defaultTileSizeHeight);
        functionalAnnotation.parseInfoTags(recordInputFileName);
        auto dataunits = functionalAnnotation.parseFunctionalAnnotation(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    } else if (recType == RecType::TRACK_FILE) {
        trackAnnotation.setCompressors(compressors);
        trackAnnotation.setTileSize(defaultTileSizeHeight);
        trackAnnotation.parseInfoTags(recordInputFileName);
        auto dataunits = trackAnnotation.parseTrack(inputfile);
        annotationParameterSet.push_back(dataunits.annotationParameterSet);
        annotationAccessUnit = dataunits.annotationAccessUnit;
    } else if (recType == RecType::TRACK_PROPERTY_FILE) {
        trackPropertyAnnotation.setCompressors(compressors);
        trackPropertyAnnotation.setTileSize(defaultTileSizeHeight);
        trackPropertyAnnotation.parseInfoTags(recordInputFileName);
        auto dataunits = trackPropertyAnnotation.parseTrackProperty(inputfile);
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
    util::BitWriter testwriter(&testfile);
    uint64_t sizeSofar = 0;

    for (auto& pars : annotationParameterSet) {
        core::record::data_unit::Record APS_dataUnit(pars);
        sizeSofar = APS_dataUnit.Write(testwriter);
    }
    for (auto& aau : annotationAccessUnit) {
        core::record::data_unit::Record AAU_dataUnit(aau);
        sizeSofar = AAU_dataUnit.Write(testwriter);
    }
    testfile.close();
}

}  // namespace annotation
}  // namespace genie

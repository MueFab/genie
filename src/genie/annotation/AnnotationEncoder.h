/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_ANNOTATIONENCODER_H_
#define SRC_GENIE_ANNOTATION_ANNOTATIONENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
// #include <map>
#include <sstream>
#include <string>
#include <vector>

// #include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
// #include "genie/util/bitreader.h"

#include "genie/annotation/Compressors.h"
#include "genie/core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"


#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/entropy/jbig/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

class AnnotationEncoder {
 public:
    core::record::annotation_parameter_set::AnnotationEncodingParameters Compose();

    void setDescriptorParameters(genie::core::AnnotDesc ID, genie::core::AlgoID encoding) {
        genie::core::record::annotation_parameter_set::AlgorithmParameters algpars;
        switch (encoding) {
            case genie::core::AlgoID::LZMA: {
                genie::entropy::lzma::LZMAParameters lzmaParameters;
                algpars = lzmaParameters.convertToAlgorithmParameters();
            } break;
            case genie::core::AlgoID::ZSTD: {
                genie::entropy::zstd::ZSTDParameters zstdParameters;
                algpars = zstdParameters.convertToAlgorithmParameters();
            } break;
            case genie::core::AlgoID::BSC: {
                genie::entropy::bsc::BSCParameters bscParameters;
                algpars = bscParameters.convertToAlgorithmParameters();
            } break;
            case genie::core::AlgoID::JBIG: {
                genie::entropy::jbig::JBIGparameters jbigParameters;
                algpars = jbigParameters.convertToAlgorithmParameters();
            }
                break;
            default:
                break;
        }

        descriptorConfigurations.emplace_back(ID, encoding, algpars);
    }

    void setDescriptorParameters(genie::core::AnnotDesc ID, genie::core::AlgoID encoding,
                                 genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmPars) {
        descriptorConfigurations.emplace_back(ID, encoding, algorithmPars);
    }

    void setDescriptors(std::vector<core::AnnotDesc> _descrList) {
        for (auto descr : _descrList) {
            setDescriptorParameters(descr, genie::core::AlgoID::BSC);
        }
    }

    void setAttributes(std::map<std::string, core::record::annotation_parameter_set::AttributeData>& info);

    void setGenotypeParameters(genotype::GenotypeParameters _parameters) {
        descriptorConfigurations.emplace_back(_parameters);
    }
    void setContactParameters(contact::ContactMatrixParameters _parameters,
                              contact::SubcontactMatrixParameters _subparameters) {
        descriptorConfigurations.emplace_back(_parameters, _subparameters);
    }
    void setLikelihoodParameters(likelihood::LikelihoodParameters _parameters) {
        descriptorConfigurations.emplace_back(_parameters);
    }

    void setCompressors(annotation::Compressor _compressors) {
        compressorParameters = _compressors.getCompressorParameters();
    }

 private:
    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptorConfigurations;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressorParameters;
    std::vector<core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_ANNOTATIONENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

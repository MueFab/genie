/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_ANNOTATION_ENCODER_H_
#define SRC_GENIE_ANNOTATION_ANNOTATION_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

#include "genie/contact/contact_matrix_parameters.h"
#include "genie/contact/subcontact_matrix_parameters.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/annotation/compressors.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/core/parameter/annotation/annotation_encoding_parameters.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/parameter/annotation/attribute_parameter_set.h"
#include "genie/core/parameter/annotation/compressor_parameter_set.h"
#include "genie/core/parameter/annotation/descriptor_configuration.h"

#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::annotation {

class AnnotationEncoder {
 public:
    core::parameter::annotation::AnnotationEncodingParameters Compose();

    void setDescriptorParameters(genie::core::AnnotDesc ID, genie::core::AlgoID encoding) {
        genie::core::parameter::annotation::AlgorithmParameters algpars;
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
            } break;
            default:
                break;
        }

        descriptorConfigurations.emplace_back(ID, encoding, algpars);
    }

    void setDescriptorParameters(genie::core::AnnotDesc ID, genie::core::AlgoID encoding,
                                 genie::core::parameter::annotation::AlgorithmParameters algorithmPars) {
        descriptorConfigurations.emplace_back(ID, encoding, algorithmPars);
    }

    void setDescriptors(std::vector<core::AnnotDesc> _descrList) {
        for (auto descr : _descrList) {
            setDescriptorParameters(descr, genie::core::AlgoID::BSC);
        }
    }

    void setAttributes(std::map<std::string, core::parameter::annotation::AttributeData>& info);

    void setGenotypeParameters(genotype::GenotypeParameters _parameters) {
        descriptorConfigurations.emplace_back(_parameters);
    }
    void setContactParameters(contact::ContactMatrixParameters _parameters,
                              std::vector<contact::SubcontactMatrixParameters> _subparameters) {
        descriptorConfigurations.emplace_back(_parameters, _subparameters);
    }
    void setLikelihoodParameters(likelihood::LikelihoodParameters _parameters) {
        descriptorConfigurations.emplace_back(_parameters);
    }

    void setCompressors(annotation::Compressor _compressors) {
        compressorParameters = _compressors.getCompressorParameters();
    }

    void setFeatures(std::vector<std::string> _features) {
        features = _features;
    }

    void setOntologyTerms(std::vector<std::string> _ontologyTerms) {
        ontologyTerms = _ontologyTerms;
    }

 private:
    std::vector<genie::core::parameter::annotation::DescriptorConfiguration> descriptorConfigurations;
    std::vector<genie::core::parameter::annotation::CompressorParameterSet> compressorParameters;
    std::vector<core::parameter::annotation::AttributeParameterSet> attribute_parameter_set;
    std::vector<std::string> features;
    std::vector<std::string> ontologyTerms;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_ANNOTATION_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

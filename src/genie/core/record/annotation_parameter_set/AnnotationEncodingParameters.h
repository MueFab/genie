/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ANNOTATIONENCODINGPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ANNOTATIONENCODINGPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AttributeParameterSet.h"
#include "CompressorParameterSet.h"
#include "DescriptorConfiguration.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

class AnnotationEncodingParameters {
 private:
    uint8_t n_filter;
    std::vector<uint8_t> filter_ID_len;
    std::vector<std::string> filter_ID;
    std::vector<uint16_t> desc_len;
    std::vector<std::string> description;

    uint8_t n_features_names;
    std::vector<uint8_t> feature_name_len;
    std::vector<std::string> feature_name;

    uint8_t n_ontology_terms;
    std::vector<uint8_t> ontology_term_name_len;
    std::vector<std::string> ontology_term_name;

    uint8_t n_descriptors;
    std::vector<DescriptorConfiguration> descriptor_configuration;

    uint8_t n_compressors;
    std::vector<CompressorParameterSet> compressor_parameter_set;

    uint8_t n_attributes;
    std::vector<AttributeParameterSet> attribute_parameter_set;

 public:
    AnnotationEncodingParameters();
    AnnotationEncodingParameters(uint8_t n_filter, std::vector<uint8_t> filter_ID_len,
                                 std::vector<std::string> filter_ID, std::vector<uint16_t> desc_len,
                                 std::vector<std::string> description, uint8_t n_features_names,
                                 std::vector<uint8_t> feature_name_len, std::vector<std::string> feature_name,
                                 uint8_t n_ontology_terms, std::vector<uint8_t> ontology_term_name_len,
                                 std::vector<std::string> ontology_term_name, uint8_t n_descriptors,
                                 std::vector<DescriptorConfiguration> descriptor_configuration, uint8_t n_compressors,
                                 std::vector<CompressorParameterSet> compressor_parameter_set, uint8_t n_attributes,
                                 std::vector<AttributeParameterSet> attribute_parameter_set);

    void read(util::BitReader& reader);
    void write(core::Writer& writer) const;

    uint8_t getNumberOfFilters() const { return n_filter; }
    std::vector<uint8_t> getFilterIDLengths() const { return filter_ID_len; }
    std::vector<std::string> getFilterIDs() const { return filter_ID; }
    std::vector<uint16_t> getDescriptionLengths() const { return desc_len; }
    std::vector<std::string> getDescriptions() const { return description; }

    uint8_t getNumberofFeatures() const { return n_features_names; }
    std::vector<uint8_t> getFeatureNameLengths() const { return feature_name_len; }
    std::vector<std::string> getFeatureNames() const { return feature_name; }

    uint8_t getNumberOfOntologyTerms() const { return n_ontology_terms; }
    std::vector<uint8_t> getOntologyTermNameLengths() const { return ontology_term_name_len; }
    std::vector<std::string> getOntologyTermName() const { return ontology_term_name; }

    uint8_t getNumberOfDescriptors() const { return n_descriptors; }
    std::vector<DescriptorConfiguration> getDescriptorConfigurations() const { return descriptor_configuration; }

    uint8_t getNumberOfCompressors() const { return n_compressors; }
    std::vector<CompressorParameterSet> getCompressorParameterSets() const { return compressor_parameter_set; }

    uint8_t getNumberOfAttributes() const { return n_attributes; }
    std::vector<AttributeParameterSet> getAttributeParameterSets() const { return attribute_parameter_set; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ANNOTATIONENCODINGPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

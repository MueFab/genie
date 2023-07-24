/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AnnotationEncodingParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {
AnnotationEncodingParameters::AnnotationEncodingParameters()
    : n_filter(0),
      filter_ID_len{},
      filter_ID{},
      desc_len{},
      description{},
      n_features_names(0),
      feature_name_len{},
      feature_name{},
      n_ontology_terms(0),
      ontology_term_name_len{},
      ontology_term_name{},
      n_descriptors(0),
      descriptor_configuration{},
      n_compressors(0),
      compressor_parameter_set{},
      n_attributes(0),
      attribute_parameter_set{} {}

AnnotationEncodingParameters::AnnotationEncodingParameters(
    uint8_t n_filter, std::vector<uint8_t> filter_ID_len, std::vector<std::string> filter_ID,
    std::vector<uint16_t> desc_len, std::vector<std::string> description, uint8_t n_features_names,
    std::vector<uint8_t> feature_name_len, std::vector<std::string> feature_name, uint8_t n_ontology_terms,
    std::vector<uint8_t> ontology_term_name_len, std::vector<std::string> ontology_term_name, uint8_t n_descriptors,
    std::vector<DescriptorConfiguration> descriptor_configuration, uint8_t n_compressors,
    std::vector<CompressorParameterSet> compressor_parameter_set, uint8_t n_attributes,
    std::vector<AttributeParameterSet> attribute_parameter_set)
    : n_filter(n_filter),
      filter_ID_len(filter_ID_len),
      filter_ID(filter_ID),
      desc_len(desc_len),
      description(description),
      n_features_names(n_features_names),
      feature_name_len(feature_name_len),
      feature_name(feature_name),
      n_ontology_terms(n_ontology_terms),
      ontology_term_name_len(ontology_term_name_len),
      ontology_term_name(ontology_term_name),
      n_descriptors(n_descriptors),
      descriptor_configuration(descriptor_configuration),
      n_compressors(n_compressors),
      compressor_parameter_set(compressor_parameter_set),
      n_attributes(n_attributes),
      attribute_parameter_set(attribute_parameter_set) {}

void AnnotationEncodingParameters::read(util::BitReader& reader) {
    n_filter = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_filter; ++i) {
        filter_ID_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string ID(filter_ID_len.back(), 0);
        for (auto& byte : ID) byte = static_cast<uint8_t>(reader.read_b(8));
        filter_ID.push_back(ID);
        desc_len.push_back(static_cast<uint16_t>(reader.read_b(10)));
        std::string desc(desc_len.back(), 0);
        for (auto& byte : desc) byte = static_cast<uint8_t>(reader.read_b(8));
        description.push_back(desc);
    }

    n_features_names = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_features_names; ++i) {
        feature_name_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string name(feature_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<uint8_t>(reader.read_b(8));
        feature_name.push_back(name);
    }

    n_ontology_terms = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_ontology_terms; ++i) {
        ontology_term_name_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string name(ontology_term_name_len.back(), 0);
        for (auto& byte : name) byte = static_cast<uint8_t>(reader.read_b(8));
        ontology_term_name.push_back(name);
    }

    n_descriptors = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_descriptors; ++i) {
        DescriptorConfiguration descrConf(reader);
        descriptor_configuration.push_back(descrConf);
    }

    n_compressors = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_compressors; ++i) {
        CompressorParameterSet comprParSet(reader);
        compressor_parameter_set.push_back(comprParSet);
    }

    n_attributes = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < n_attributes; ++i) {
        AttributeParameterSet attrParSet(reader);
        attribute_parameter_set.push_back(attrParSet);
    }
    reader.flush();
}

void AnnotationEncodingParameters::write(core::Writer& writer) const {
    writer.write(n_filter, 8);
    for (auto i = 0; i < n_filter; ++i) {
        writer.write(filter_ID_len[i], 6);
        for (auto byte : filter_ID[i]) writer.write(byte, 8);
        writer.write(desc_len[i], 10);
        for (auto byte : description[i]) writer.write(byte, 8);
    }
    writer.write(n_features_names, 8);
    for (auto i = 0; i < n_features_names; ++i) {
        writer.write(feature_name_len[i], 6);
        for (auto byte : feature_name[i]) writer.write(byte, 8);
    }
    writer.write(n_ontology_terms, 8);
    for (auto i = 0; i < n_ontology_terms; ++i) {
        writer.write(ontology_term_name_len[i], 6);
        for (auto byte : ontology_term_name[i]) writer.write(byte, 8);
    }
    writer.write(n_descriptors, 8);
    for (auto i = 0; i < n_descriptors; ++i) {
        descriptor_configuration[i].write(writer);
    }
    writer.write(n_compressors, 8);
    for (auto i = 0; i < n_compressors; ++i) {
        compressor_parameter_set[i].write(writer);
    }
    writer.write(n_attributes, 8);
    for (auto i = 0; i < n_attributes; ++i) {
        attribute_parameter_set[i].write(writer);
    }
    writer.flush();
}

size_t AnnotationEncodingParameters::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

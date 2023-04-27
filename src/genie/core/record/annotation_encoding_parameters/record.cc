/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/core/record/variant_genotype/arrayType.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {


CompressorParameterSet::CompressorParameterSet(util::BitReader& reader) { read(reader); }

void CompressorParameterSet::read(util::BitReader& reader) {
    compressor_ID = reader.readBypassBE<uint8_t>();
    n_compressor_steps = static_cast<uint8_t>(reader.read_b(4));
    for (auto i = 0; i < n_compressor_steps; ++i) {
        compressor_step_ID.push_back(static_cast<uint8_t>(reader.read_b(4)));
        algorithm_ID.push_back(static_cast<uint8_t>(reader.read_b(5)));
        use_default_pars.push_back(static_cast<bool>(reader.read_b(1)));
        if (use_default_pars.back()) algorithm_parameters.read(reader);
    }
}

DescriptorConfiguration::DescriptorConfiguration(util::BitReader& reader) { read(reader); }

void DescriptorConfiguration::read(util::BitReader& reader) {
    descriptor_ID = static_cast<DescriptorID>(reader.readBypassBE<uint8_t>());
    encoding_mode_ID = reader.readBypassBE<uint8_t>();
    switch (descriptor_ID) {
        case DescriptorID::GENOTYPE:
            genotype_parameters.read(reader);
            break;
        case DescriptorID::LIKELIHOOD:
            likelihood_parameters.read(reader);
            break;
        case DescriptorID::CONTACT:
            contact_matrix_parameters.read(reader);
            break;
        default:
            break;
    }
    algorithm_patarmeters.read(reader);
}

void Record::read(util::BitReader& reader) {
    n_filter = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_filter; ++i) {
        filter_ID_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string ID(filter_ID_len.back(), 0);
        reader.readBypass(ID);
        filter_ID.push_back(ID);
        desc_len.push_back(static_cast<uint16_t>(reader.read_b(10)));
        std::string desc(desc_len.back(), 0);
        description.push_back(desc);
    }

    n_features_names = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_features_names; ++i) {
        feature_name_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string name(feature_name_len.back(), 0);
        reader.readBypass(name);
        feature_name.push_back(name);
    }

    n_ontology_terms = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_ontology_terms; ++i) {
        ontology_term_name_len.push_back(static_cast<uint8_t>(reader.read_b(6)));
        std::string name(ontology_term_name_len.back(), 0);
        reader.readBypass(name);
        ontology_term_name.push_back(name);
    }

    n_descriptors = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_descriptors; ++i) {
        DescriptorConfiguration descrConf(reader);
        descriptor_configuration.push_back(descrConf);
    }

    n_compressors = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_compressors; ++i) {
        CompressorParameterSet comprParSet(reader);
        compressor_parameter_set.push_back(comprParSet);
    }

    n_attributes = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < n_compressors; ++i) {
        AttributeParameterSet attrParSet(reader);
        attribute_parameter_set.push_back(attrParSet);
    }
}

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

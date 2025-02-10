/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/annotation/annotation_encoder.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// #include "genie/contact/contact_matrix_parameters.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"
#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie/core/record/annotation_parameter_set/TileStructure.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/variantsite/parameterset_composer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {
core::record::annotation_parameter_set::AnnotationEncodingParameters AnnotationEncoder::Compose() {
    std::vector<core::record::annotation_parameter_set::filterData> filters;
    std::vector<std::string> features;
    std::vector<std::string> ontologyTerms;

    core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
        filters, features, ontologyTerms, descriptorConfigurations, compressorParameters, attribute_parameter_set);

    return annotation_encoding_parameters;
}

void AnnotationEncoder::setAttributes(
    std::map<std::string, core::record::annotation_parameter_set::AttributeData>& info) {
    // uint8_t n_attributes = static_cast<uint8_t>(info.size());
    for (auto it = info.begin(); it != info.end(); ++it) {
        core::ArrayType deftype;
        uint16_t attrID = info[it->first].getAttributeID();
        std::string attribute_name = info[it->first].getAttributeName();
        core::DataType attribute_type = info[it->first].getAttributeType();
        std::vector<uint8_t> attribute_array_dims;
        if (info[it->first].getArrayLength() == 1) {
        } else {
            attribute_array_dims.push_back(info[it->first].getArrayLength());
        }
        std::vector<uint8_t> attribute_default_val =
            deftype.toArray(attribute_type, deftype.getDefaultValue(attribute_type));
        bool attribute_miss_val_flag = true;
        bool attribute_miss_default_flag = false;
        std::vector<uint8_t> attribute_miss_val = attribute_default_val;
        std::string attribute_miss_str = "";

        core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet(
            attrID, attribute_name, attribute_type, attribute_array_dims,
            attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag, attribute_miss_val,
            attribute_miss_str, 1);
        attribute_parameter_set.push_back(attributeParameterSet);
    }
}

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

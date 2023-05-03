/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATIONENCODINGPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATIONENCODINGPARAMETERS_H_

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
#include "TileConfiguration.h"
#include "TileStructure.h"
#include "DescriptorConfiguration.h"
#include "CompressorParameterSet.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {




/**
 *  @brief
 */
class Record {
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
    /**
     * @brief
     */
    Record();
    void read(util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATIONENCODINGPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

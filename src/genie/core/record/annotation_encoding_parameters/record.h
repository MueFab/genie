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

#include "LikelihoodParameters.h"
#include "GenotypeParameters.h"
#include "AttributeParameterSet.h"
#include "AlgorithmParameters.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

enum class DescriptorID {
    SEQUENCEID = 1,
    STARTPOS,
    ENDPOS,
    STRAND,
    NAME,
    DESCRIPTION,
    LINKNAME,
    LINKID,
    DEPTH,
    SEQQUALITY,
    MAPQUALITY,
    MAPNUMQUALITY0,
    REFERENCE,
    ALTERN,
    GENOTYPE,
    LIKELIHOOD,
    FILTER,
    FEATURENAME,
    FEATUREID,
    ONTOLOGYNAME,
    ONTOLOGYID,
    CONTACT,
    ATTRIBUTE = 31
};



class CompressorParameterSet {
 private:
    uint8_t compressor_ID;
    uint8_t n_compressor_steps;
    std::vector<uint8_t> compressor_step_ID;
    std::vector<uint8_t> algorithm_ID;
    std::vector<bool> use_default_pars;
    AlgorithmParameters algorithm_parameters;
    std::vector<uint8_t> n_in_vars;
    std::vector<std::vector<uint8_t>> in_var_ID;
    std::vector<std::vector<uint8_t>> prev_step_ID;
    std::vector<std::vector<uint8_t>> prev_out_var_ID;
    std::vector<uint8_t> n_completed_out_vars;
    std::vector<std::vector<uint8_t>> completed_out_var_ID;

 public:
    CompressorParameterSet(util::BitReader& reader);
    void read(util::BitReader& reader);
};


class ContactMatrixParameters {
 private:
    uint8_t num_samples{};
    std::vector<uint8_t> sample_ID{};
    std::vector<std::string> sample_name{};
    uint8_t num_chrs{};
    std::vector<uint8_t> chr_ID{};
    std::vector<std::string> chr_name{};
    std::vector<uint64_t> chr_length{};

    uint32_t interval{};
    uint32_t tile_size{};
    uint8_t num_interval_multipliers{};
    std::vector<uint32_t> interval_multiplier{};

    uint8_t num_norm_methods{};
    std::vector<uint8_t> norm_method_ID{};
    std::vector<std::string> norm_method_name{};
    std::vector<bool> norm_method_mult_flag{};

    uint8_t num_norm_matrices{};
    std::vector<uint8_t> norm_matrix_ID{};
    std::vector<std::string> norm_matrix_name{};

 public:
    void read(util::BitReader& reader);
};

class DescriptorConfiguration {
 private:
    DescriptorID descriptor_ID;
    uint8_t encoding_mode_ID;
    GenotypeParameters genotype_parameters;
    LikelihoodParameters likelihood_parameters;
    ContactMatrixParameters contact_matrix_parameters;
    AlgorithmParameters algorithm_patarmeters;

 public:
    DescriptorConfiguration(util::BitReader& reader);
    void read(util::BitReader& reader);
};

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

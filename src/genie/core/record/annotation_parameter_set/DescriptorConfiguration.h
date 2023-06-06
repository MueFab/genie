/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_DESCRIPTORCONFIGURATION_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_DESCRIPTORCONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "AlgorithmParameters.h"
#include "ContactMatrixParameters.h"
#include "GenotypeParameters.h"
#include "LikelihoodParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

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

class DescriptorConfiguration {
 private:
    DescriptorID descriptor_ID;
    uint8_t encoding_mode_ID;
    GenotypeParameters genotype_parameters;
    LikelihoodParameters likelihood_parameters;
    ContactMatrixParameters contact_matrix_parameters;
    AlgorithmParameters algorithm_patarmeters;

 public:
    DescriptorConfiguration();
    explicit DescriptorConfiguration(util::BitReader& reader);
    DescriptorConfiguration(DescriptorID descriptor_ID, uint8_t encoding_mode_ID,
                            GenotypeParameters genotype_parameters, LikelihoodParameters likelihood_parameters,
                            ContactMatrixParameters contact_matrix_parameters,
                            AlgorithmParameters algorithm_patarmeters);
    void read(util::BitReader& reader);
    void write(std::ostream& outputfile) const;
    void write(util::BitWriter& writer) const;

    DescriptorID getDescriptorID() const { return descriptor_ID; }
    uint8_t getEncodingModeID() const { return encoding_mode_ID; }
    GenotypeParameters getGenotypeParameters() const { return genotype_parameters; }
    LikelihoodParameters getLikelihoodParameters() const { return likelihood_parameters; }
    ContactMatrixParameters getContactMatrixParameters() const { return contact_matrix_parameters; }
    AlgorithmParameters getAlgorithmParameters() const { return algorithm_patarmeters; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_DESCRIPTORCONFIGURATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

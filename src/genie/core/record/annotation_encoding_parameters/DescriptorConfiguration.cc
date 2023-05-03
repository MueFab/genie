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

#include "DescriptorConfiguration.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {
DescriptorConfiguration::DescriptorConfiguration() : descriptor_ID(DescriptorID::GENOTYPE), encoding_mode_ID(0) {}
DescriptorConfiguration::DescriptorConfiguration(util::BitReader& reader) { read(reader); }

DescriptorConfiguration::DescriptorConfiguration(DescriptorID descriptor_ID, uint8_t encoding_mode_ID,
                                                 GenotypeParameters genotype_parameters,
                                                 LikelihoodParameters likelihood_parameters,
                                                 ContactMatrixParameters contact_matrix_parameters,
                                                 AlgorithmParameters algorithm_patarmeters)
    : descriptor_ID(descriptor_ID),
      encoding_mode_ID(encoding_mode_ID),
      genotype_parameters(genotype_parameters),
      likelihood_parameters(likelihood_parameters),
      contact_matrix_parameters(contact_matrix_parameters),
      algorithm_patarmeters(algorithm_patarmeters) {}

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

void DescriptorConfiguration::write(std::ostream& outputfile) const {
    outputfile << std::to_string(static_cast<uint8_t>(descriptor_ID)) << ",";
    outputfile << std::to_string(encoding_mode_ID) << ",";
    if (descriptor_ID == DescriptorID::GENOTYPE)
        genotype_parameters.write(outputfile);
    else if (descriptor_ID == DescriptorID::LIKELIHOOD)
        likelihood_parameters.write(outputfile);
    else if (descriptor_ID == DescriptorID::CONTACT)
        contact_matrix_parameters.write(outputfile);
    algorithm_patarmeters.write(outputfile);
}

void DescriptorConfiguration::write(util::BitWriter& writer) const {
    writer.write(static_cast<uint8_t>(descriptor_ID), 8);
    writer.write(encoding_mode_ID, 8);
    if (descriptor_ID == DescriptorID::GENOTYPE)
        genotype_parameters.write(writer);
    else if (descriptor_ID == DescriptorID::LIKELIHOOD)
        likelihood_parameters.write(writer);
    else if (descriptor_ID == DescriptorID::CONTACT)
        contact_matrix_parameters.write(writer);
    algorithm_patarmeters.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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
namespace annotation_parameter_set {
DescriptorConfiguration::DescriptorConfiguration() : descriptor_ID(AnnotDesc::GENOTYPE), encoding_mode_ID(0) {}
DescriptorConfiguration::DescriptorConfiguration(util::BitReader& reader) { read(reader); }

DescriptorConfiguration::DescriptorConfiguration(AnnotDesc descriptor_ID, uint8_t encoding_mode_ID,
                                                 GenotypeParameters genotype_parameters,
                                                 LikelihoodParameters likelihood_parameters,
                                                 ContactMatrixParameters contact_matrix_parameters,
                                                 AlgorithmParameters algorithm_parameters)
    : descriptor_ID(descriptor_ID),
      encoding_mode_ID(encoding_mode_ID),
      genotype_parameters(genotype_parameters),
      likelihood_parameters(likelihood_parameters),
      contact_matrix_parameters(contact_matrix_parameters),
      algorithm_parameters(algorithm_parameters) {}

void DescriptorConfiguration::read(util::BitReader& reader) {
    descriptor_ID = static_cast<AnnotDesc>(static_cast<uint8_t>(reader.read_b(8)));
    encoding_mode_ID = static_cast<uint8_t>(reader.read_b(8));
    switch (descriptor_ID) {
        case AnnotDesc::GENOTYPE:
            genotype_parameters.read(reader);
            break;
        case AnnotDesc::LIKELIHOOD:
            likelihood_parameters.read(reader);
            break;
        case AnnotDesc::CONTACT:
            contact_matrix_parameters.read(reader);
            break;
        default:
            break;
    }
    algorithm_parameters.read(reader);
}

void DescriptorConfiguration::write(core::Writer& writer) const {
    writer.write(static_cast<uint8_t>(descriptor_ID), 8);
    writer.write(encoding_mode_ID, 8);
    if (descriptor_ID == AnnotDesc::GENOTYPE)
        genotype_parameters.write(writer);
    else if (descriptor_ID == AnnotDesc::LIKELIHOOD)
        likelihood_parameters.write(writer);
    else if (descriptor_ID == AnnotDesc::CONTACT)
        contact_matrix_parameters.write(writer);
    algorithm_parameters.write(writer);
}

size_t DescriptorConfiguration::getSize(core::Writer& write_size) const {
    write(write_size);
    return write_size.getBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

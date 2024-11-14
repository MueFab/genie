/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------
#include "DescriptorConfiguration.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "genie/likelihood/likelihood_parameters.h"
#include "genie/contact/contact_matrix_parameters.h"
#include "genie/contact/subcontact_matrix_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

DescriptorConfiguration::DescriptorConfiguration()
    : descriptor_ID(AnnotDesc::GENOTYPE), encoding_mode_ID(AlgoID::CABAC) {}

DescriptorConfiguration::DescriptorConfiguration(util::BitReader& reader) { read(reader); }

DescriptorConfiguration::DescriptorConfiguration(AnnotDesc _descriptor_ID, AlgoID _encoding_mode_ID,
                                                 AlgorithmParameters _algorithm_parameters)
    : descriptor_ID(_descriptor_ID), encoding_mode_ID(_encoding_mode_ID), algorithm_parameters(_algorithm_parameters) {}

DescriptorConfiguration::DescriptorConfiguration(genie::genotype::GenotypeParameters _genotype_parameters)
    : DescriptorConfiguration(genie::core::AnnotDesc::GENOTYPE, genie::core::AlgoID::ZSTD, AlgorithmParameters{}) {
    genotype_parameters = _genotype_parameters;
}

DescriptorConfiguration::DescriptorConfiguration(genie::likelihood::LikelihoodParameters _likelihood_parameters)
    : DescriptorConfiguration(genie::core::AnnotDesc::LIKELIHOOD, genie::core::AlgoID::ZSTD, AlgorithmParameters{}) {
    likelihood_parameters = _likelihood_parameters;
}

DescriptorConfiguration::DescriptorConfiguration( genie::contact::ContactMatrixParameters _contact_matrix_parameters, genie::contact::SubcontactMatrixParameters _subconstract_matrix_parameters)
    : DescriptorConfiguration(genie::core::AnnotDesc::CONTACT, genie::core::AlgoID::ZSTD, AlgorithmParameters{}) {
    contact_matrix_parameters = _contact_matrix_parameters;
    subcontract_matrix_parameters = _subconstract_matrix_parameters;
}

void DescriptorConfiguration::read(util::BitReader& reader) {
    descriptor_ID = static_cast<AnnotDesc>(static_cast<uint8_t>(reader.read_b(8)));
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_parameters.read(reader);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_parameters.read(reader);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        // not implemented
    } else {
        encoding_mode_ID = static_cast<AlgoID>(reader.read_b(8));
        algorithm_parameters.read(reader);
    }
}

void DescriptorConfiguration::write(core::Writer& writer) const {
    writer.write(static_cast<uint8_t>(descriptor_ID), 8);
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_parameters.write(writer);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_parameters.write(writer);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        contact_matrix_parameters.write(writer);       
        subcontract_matrix_parameters.write(writer.getBinWriter());
    } else {
        writer.write(static_cast<uint8_t>(encoding_mode_ID), 8);
        algorithm_parameters.write(writer);
    }
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

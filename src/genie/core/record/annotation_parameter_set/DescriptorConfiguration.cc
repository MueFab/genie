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

DescriptorConfiguration::DescriptorConfiguration(AnnotDesc _descriptor_ID, AlgoID _encoding_mode_ID,
                                                 genie::genotype::GenotypeParameters _genotype_parameters,
                                                 AlgorithmParameters _algorithm_parameters)
    : DescriptorConfiguration(_descriptor_ID, _encoding_mode_ID, _algorithm_parameters) {
    genotype_parameters = _genotype_parameters;
}

DescriptorConfiguration::DescriptorConfiguration(AnnotDesc _descriptor_ID, AlgoID _encoding_mode_ID,
                                                 genie::likelihood::LikelihoodParameters _likelihood_parameters,
                                                 AlgorithmParameters _algorithm_parameters)
    : DescriptorConfiguration(_descriptor_ID, _encoding_mode_ID, _algorithm_parameters) {
    likelihood_parameters = _likelihood_parameters;
}

DescriptorConfiguration::DescriptorConfiguration(AnnotDesc _descriptor_ID, AlgoID _encoding_mode_ID,
                                                 genie::contact::ContactMatrixParameters _contact_matrix_parameters,
                                                 AlgorithmParameters _algorithm_parameters)
    : DescriptorConfiguration(_descriptor_ID, _encoding_mode_ID, _algorithm_parameters) {
    contact_matrix_parameters = _contact_matrix_parameters;
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
        // not implemented
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

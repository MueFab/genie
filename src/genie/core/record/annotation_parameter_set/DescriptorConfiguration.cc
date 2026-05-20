/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "DescriptorConfiguration.h"

#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

DescriptorConfiguration::DescriptorConfiguration()
    : descriptor_ID(AnnotDesc::GENOTYPE), encoding_mode_ID(AlgoID::CABAC) {}

DescriptorConfiguration::DescriptorConfiguration(util::BitReader& reader) { Read(reader); }

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

DescriptorConfiguration::DescriptorConfiguration(
    genie::contact::ContactMatrixParameters _contact_matrix_parameters,
    std::vector<genie::contact::SubcontactMatrixParameters> _subconstract_matrix_parameters)
    : DescriptorConfiguration(genie::core::AnnotDesc::CONTACT, genie::core::AlgoID::ZSTD, AlgorithmParameters{}) {
    contact_matrix_parameters = _contact_matrix_parameters;
    subcontract_matrix_parameters = _subconstract_matrix_parameters;
}

void DescriptorConfiguration::Read(util::BitReader& reader) {
    descriptor_ID = static_cast<AnnotDesc>(static_cast<uint8_t>(reader.ReadBits(8)));
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_parameters.Read(reader);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_parameters.Read(reader);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        // not implemented
        // contact_matrix_parameters.Read(reader);
    } else {
        encoding_mode_ID = static_cast<AlgoID>(reader.ReadBits(8));
        algorithm_parameters.Read(reader);
    }
}

void DescriptorConfiguration::Write(util::BitWriter& writer) const {
    writer.WriteBits(static_cast<uint8_t>(descriptor_ID), 8);
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_parameters.Write(writer);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_parameters.Write(writer);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        contact_matrix_parameters.Write(writer);
        writer.WriteBits(subcontract_matrix_parameters.size(), 16);
        for (auto& scm_params : subcontract_matrix_parameters)
            scm_params.Write(writer);
    } else {
        writer.WriteBits(static_cast<uint8_t>(encoding_mode_ID), 8);
        algorithm_parameters.Write(writer);
    }
}

size_t DescriptorConfiguration::GetSize(util::BitWriter& write_size) const {
    Write(write_size);
    return write_size.GetTotalBitsWritten();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

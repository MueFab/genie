/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "arrayType.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

void FormatField::write(util::BitReader& reader, uint32_t sample_count) {
    len = (reader.readBypassBE<uint8_t>());
    format.resize(len);
    reader.readBypass(&format[0], len);
    type = (reader.readBypassBE<uint8_t>());
    arrayType curType;
    array_len = (reader.readBypassBE<uint8_t>());
    value_array.resize(sample_count);
    for (auto& values : value_array) {
        values.resize(array_len);
        for (auto& value : values) {
            value = curType.toArray(type, reader);
        }
    }
}

Record::Record(util::BitReader& reader) { read(reader); }

void Record::read(util::BitReader& reader) {
    variant_index = (reader.readBypassBE<uint64_t>());
    sample_index_from = (reader.readBypassBE<uint32_t>());
    sample_count = (reader.readBypassBE<uint32_t>());

    format_count = (reader.readBypassBE<uint8_t>());
    if (format_count > 0) {
        format.resize(format_count);
        for (auto& format_item : format) {
            format_item.write(reader, sample_count);
        }
    }
    genotype_present = (reader.readBypassBE<uint8_t>());
    likelihood_present = (reader.readBypassBE<uint8_t>());

    n_alleles_per_sample = (reader.readBypassBE<uint8_t>());
    alleles.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample));
    phasing.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample - 1));
    for (uint32_t i = 0; i < sample_count; ++i) {
        for (auto& allele : alleles[i]) {
            allele = (reader.readBypassBE<uint8_t>());
        }
        for (auto& phase : phasing[i]) {
            phase = (reader.readBypassBE<uint8_t>());
        }
    }

    n_likelihoods = (reader.readBypassBE<uint8_t>());
    likelihoods.resize(sample_count, std::vector<float>(n_likelihoods));
    for (auto& likelihood_sample : likelihoods) {
        for (auto& likelihood : likelihood_sample) {
            likelihood = (reader.readBypassBE<float>());
        }
    }

    linked_record = (reader.readBypassBE<uint8_t>());
    if (linked_record) {
        link_name_len = (reader.readBypassBE<uint8_t>());
        reader.readBypass(&link_name[0], link_name_len);
        reference_box_ID = (reader.readBypassBE<uint8_t>());
    }
}

void Record::write(std::ostream& outputfile) const {
    outputfile << std::to_string(variant_index) << ",";
    outputfile << std::to_string(sample_index_from) << ",";
    outputfile << std::to_string(sample_count) << ",";

    outputfile << std::to_string(format_count) << ",";

    arrayType curArray;

    for (auto format_item : format) {
        outputfile << std::to_string(format_item.len) << ",";
        outputfile << '"' << format_item.format << '"' << ",";
        outputfile << std::to_string(format_item.type) << ",";
        outputfile << std::to_string(format_item.array_len) << ",";
        for (auto values : format_item.value_array) {
            for (auto value : values) {
                outputfile << curArray.toString(format_item.type, value) << ",";
            }
        }
    }
    outputfile << std::to_string(genotype_present) << ",";
    outputfile << std::to_string(likelihood_present) << ",";

    outputfile << std::to_string(n_alleles_per_sample) << ",";

    for (auto allele_sample : alleles) {
        for (auto& allele : allele_sample) {
            outputfile << std::to_string(allele) << ",";
        }
    }

    for (auto& phasing_sample : phasing) {
        for (auto& phase : phasing_sample) {
            outputfile << std::to_string(phase) << ",";
        }
    }

    outputfile << std::to_string(n_likelihoods) << ",";

    for (auto likelihood_sample : likelihoods) {
        for (auto likelihood : likelihood_sample) {
            outputfile << std::to_string(likelihood) << ",";
        }
    }

    outputfile << std::to_string(linked_record) << ",";
    if (linked_record) {
        outputfile << std::to_string(link_name_len) << ",";
        outputfile << '"' << link_name << '"' << ",";
        outputfile << std::to_string(reference_box_ID) << ",";
    }
}

uint64_t Record::getVariantIndex() const { return variant_index; }

uint32_t Record::getStartSampleIndex() const { return sample_index_from; }

uint32_t Record::getSampleCount() const { return sample_count; }

uint8_t Record::getFormatCount() const { return format_count; }

std::vector<FormatField> Record::getFormat() const { return format; }

bool Record::isGenotypePresent() const { return (genotype_present == 0 ? false : true); }

bool Record::isLikelihoodPresent() const { return (likelihood_present) == 0 ? false : true; }

uint8_t Record::getNumberOfAllelesPerSample() const { return n_alleles_per_sample; }

std::vector<std::vector<uint8_t>> Record::getAlleles() const { return alleles; }

std::vector<std::vector<uint8_t>> Record::getPhasing() const { return phasing; }

uint8_t Record::getNumberOfLikelihoods() const { return n_likelihoods; }

std::vector<std::vector<float>> Record::getLikelihoods() const { return likelihoods; }

bool Record::isLinkedRecord() const { return (linked_record == 0 ? false : true); }

std::string Record::getLinkName() const { return link_name; }

uint8_t Record::getReferenceBoxID() const { return reference_box_ID; }

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

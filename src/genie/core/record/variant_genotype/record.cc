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

#include "record.h"
#include "arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

uint8_t Record::determineSize(uint8_t selectType) const {
    if (selectType == 0) return 0;
    if (selectType < 5) return 1;
    if (selectType < 7) return 2;
    if (selectType < 9 || selectType == 11) return 4;
    return 4;
}


Record::Record(util::BitReader& reader) : reader(reader) {
    variant_index = (reader.readBypassBE<uint64_t>());
    sample_index_from = (reader.readBypassBE<uint32_t>());
    sample_count = (reader.readBypassBE<uint32_t>());

    arrayType curType;

    format_count = (reader.readBypassBE<uint8_t>());
    if (format_count > 0) {
        format.resize(format_count);
        for (auto& format_item : format) {
            format_item.len = (reader.readBypassBE<uint8_t>());
            format_item.format.resize(format_item.len);
            reader.readBypass(&format_item.format[0], format_item.len);
            format_item.type = (reader.readBypassBE<uint8_t>());
            format_item.array_len = (reader.readBypassBE<uint8_t>());
            format_item.value.resize(sample_count);
            for (auto& values : format_item.value) {
                values.resize(format_item.array_len);
                for (auto& value : values) {
                    value = curType.toArray(format_item.type, reader);
                }
            }
        }
    }
    genotype_present = (reader.readBypassBE<uint8_t>());
    likelihood_present = (reader.readBypassBE<uint8_t>());

    n_alleles_per_sample = (reader.readBypassBE<uint8_t>());
    alleles.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample));
    phasing.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample-1));
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
        for (auto values : format_item.value) {
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

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

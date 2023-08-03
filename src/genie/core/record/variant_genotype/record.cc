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

#include "genie/core/arrayType.h"
#include "genie/core/record/variant_genotype/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

//void format_field::write(util::BitReader& reader, uint32_t sample_count) {
//    len = (reader.readBypassBE<uint8_t>());
//    format.resize(len);
//    reader.readBypass(&format[0], len);
//    type = (reader.readBypassBE<uint8_t>());
//    arrayType curType;
//    array_len = (reader.readBypassBE<uint8_t>());
//    value_array.resize(sample_count);
//    for (auto& values : value_array) {
//        values.resize(array_len);
//        for (auto& value : values) {
//            value = curType.toArray(type, reader);
//        }
//    }
//}

//VariantGenotype::VariantGenotype(uint64_t variant_index, uint32_t sample_index_from, uint32_t sample_count, uint8_t format_count,
//               std::vector<format_field> format, uint8_t genotype_present, uint8_t likelihood_present,
//               uint8_t n_alleles_per_sample, std::vector<std::vector<uint8_t>> alleles,
//               std::vector<std::vector<uint8_t>> phasings, uint8_t n_likelihoods,
//               std::vector<std::vector<uint32_t>> likelihoods, uint8_t linked_record, uint8_t link_name_len,
//               std::string link_name, uint8_t reference_box_ID)
//    : variant_index(variant_index),
//      sample_index_from(sample_index_from),
//      sample_count(sample_count),
//      format_count(format_count),
//      format(format),
//      genotype_present(genotype_present),
//      likelihood_present(likelihood_present),
//      n_alleles_per_sample(n_alleles_per_sample),
//      alleles(alleles),
//      phasings(phasings),
//      n_likelihoods(n_likelihoods),
//      likelihoods(likelihoods),
//      linked_record(linked_record),
//      link_name_len(link_name_len),
//      link_name(link_name),
//      reference_box_ID(reference_box_ID) {}

VariantGenotype::VariantGenotype(util::BitReader& bitreader)
    : variant_index(bitreader.readBypassBE<uint64_t>()),
      sample_index_from(bitreader.readBypassBE<uint32_t>()),
      sample_count(bitreader.readBypassBE<uint32_t>()),
      format(),
      alleles(),
      phasings()
    {

    auto format_count = bitreader.readBypassBE<uint8_t>();
    if (format_count > 0)
        UTILS_DIE("FORMAT parser is not yet implement!");
//    for (uint8_t i = 0; i < format_count; i++){
//        format.emplace_back(bitreader);
//    }

    bool genotype_present = bitreader.read<bool>(8);
    bool likelihood_present = bitreader.read<bool>(8);

    if (genotype_present){
        auto n_alleles_per_sample = bitreader.read<uint8_t>(8);

        alleles.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample));
        phasings.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample - 1));

        for (auto& alleles_sample : alleles) {
            for (auto& allele : alleles_sample) {
                allele = bitreader.readBypassBE<uint8_t>();
            }
        }

        if (n_alleles_per_sample-1 > 0){
            for (auto& phasings_sample : phasings) {
                for (auto& phasing : phasings_sample) {
                    phasing = bitreader.readBypassBE<uint8_t>();
                }
            }
        }
    }

    if (likelihood_present){
        auto n_likelihoods = bitreader.readBypassBE<uint8_t>();

        likelihoods.resize(sample_count, std::vector<uint32_t>(n_likelihoods));

        for (auto& likelihood_sample : likelihoods) {
            for (auto& likelihood : likelihood_sample) {
                    likelihood = bitreader.readBypassBE<uint32_t>();
            }
        }
    }

    auto linked_record = bitreader.read<bool>(8);
    if (linked_record) {
        link_record = LinkRecord(bitreader);
    }
}

//void VariantGenotype::write(std::ostream& outputfile) const {
//    outputfile << std::to_string(variant_index) << ",";
//    outputfile << std::to_string(sample_index_from) << ",";
//    outputfile << std::to_string(sample_count) << ",";
//
//    outputfile << std::to_string(format_count) << ",";
//
//    arrayType curArray;
//
//    for (auto format_item : format) {
//        outputfile << std::to_string(format_item.len) << ",";
//        outputfile << '"' << format_item.format << '"' << ",";
//        outputfile << std::to_string(format_item.type) << ",";
//        outputfile << std::to_string(format_item.array_len) << ",";
//        for (auto values : format_item.value_array) {
//            for (auto value : values) {
//                outputfile << curArray.toString(format_item.type, value) << ",";
//            }
//        }
//    }
//    outputfile << std::to_string(genotype_present) << ",";
//    outputfile << std::to_string(likelihood_present) << ",";
//
//    outputfile << std::to_string(n_alleles_per_sample) << ",";
//
//    for (auto allele_sample : alleles) {
//        for (auto& allele : allele_sample) {
//            outputfile << std::to_string(allele) << ",";
//        }
//    }
//
//    for (auto& phasing_sample : phasings) {
//        for (auto& phase : phasing_sample) {
//            outputfile << std::to_string(phase) << ",";
//        }
//    }
//
//    outputfile << std::to_string(n_likelihoods) << ",";
//
//    for (auto likelihood_sample : likelihoods) {
//        for (auto likelihood : likelihood_sample) {
//            outputfile << std::to_string(likelihood) << ",";
//        }
//    }
//
//    outputfile << std::to_string(linked_record) << ",";
//    if (linked_record) {
//        outputfile << std::to_string(link_name_len) << ",";
//        outputfile << '"' << link_name << '"' << ",";
//        outputfile << std::to_string(reference_box_ID) << ",";
//    }
//}

uint64_t VariantGenotype::getVariantIndex() const { return variant_index; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::getStartSampleIndex() const { return sample_index_from; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::getSampleCount() const { return sample_count; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getFormatCount() const { return format.size(); }

// ---------------------------------------------------------------------------------------------------------------------

//
//std::vector<format_field> VariantGenotype::getFormat() const { return format; }
//

// ---------------------------------------------------------------------------------------------------------------------


bool VariantGenotype::isGenotypePresent() const {
    return !alleles.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

bool VariantGenotype::isLikelihoodPresent() const {
    return !likelihoods.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getNumberOfAllelesPerSample() const {
    if (isGenotypePresent()){
        return alleles[0].size();
    } else {
        return 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint8_t>>& VariantGenotype::getAlleles() const { return alleles; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint8_t>>& VariantGenotype::getPhasing() const { return phasings; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getNumberOfLikelihoods() const {
    if (isLikelihoodPresent()){
        return 0;
    } else {
        return likelihoods[0].size();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint32_t>>& VariantGenotype::getLikelihoods() const { return likelihoods; }

// ---------------------------------------------------------------------------------------------------------------------

bool VariantGenotype::getLinkedRecord() const {return link_record != boost::none;}

// ---------------------------------------------------------------------------------------------------------------------

const boost::optional<LinkRecord>& VariantGenotype::getLinkRecord() const {return link_record;}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

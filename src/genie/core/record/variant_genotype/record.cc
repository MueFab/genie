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

VariantGenotype::VariantGenotype(util::BitReader& bitreader)
    : variant_index(bitreader.readBypassBE<uint64_t>()),
      sample_index_from(bitreader.readBypassBE<uint32_t>()),
      sample_count(bitreader.readBypassBE<uint32_t>()),
      format(),
      alleles(),
      phasings() {

    auto format_count = bitreader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i < format_count; i++) {
        format.emplace_back(bitreader, sample_count);
    }

    bool genotype_present = bitreader.read<bool>(8);
    bool likelihood_present = bitreader.read<bool>(8);

    if (genotype_present) {
        auto n_alleles_per_sample = bitreader.read<uint8_t>(8);
        UTILS_DIE_IF(n_alleles_per_sample == 0, "Invalid n_alleles_per_sample!");

        alleles.resize(sample_count, std::vector<int8_t>(n_alleles_per_sample));
        phasings.resize(sample_count, std::vector<int8_t>(n_alleles_per_sample - 1));

        for (auto& alleles_sample : alleles) {
            for (auto& allele : alleles_sample) {
                allele = bitreader.readBypassBE<uint8_t>();
            }
        }

        if (n_alleles_per_sample - 1 > 0) {
            for (auto& phasings_sample : phasings) {
                for (auto& phasing : phasings_sample) {
                    phasing = bitreader.readBypassBE<uint8_t>();
                }
            }
        }
    }

    if (likelihood_present) {
        auto n_likelihoods = bitreader.readBypassBE<uint8_t>();
        UTILS_DIE_IF(n_likelihoods == 0, "Invalid n_likelihoods!");

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

uint64_t VariantGenotype::getVariantIndex() const { return variant_index; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::getStartSampleIndex() const { return sample_index_from; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::getNumSamples() const { return sample_count; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getFormatCount() const { return static_cast<uint8_t>(format.size()); }

// ---------------------------------------------------------------------------------------------------------------------

//
// std::vector<format_field> VariantGenotype::getFormat() const { return format; }
//

// ---------------------------------------------------------------------------------------------------------------------

bool VariantGenotype::isGenotypePresent() const { return !alleles.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

bool VariantGenotype::isLikelihoodPresent() const { return !likelihoods.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getNumberOfAllelesPerSample() const {
    if (isGenotypePresent()) {
        return static_cast<uint8_t>(alleles.front().size());
    } else {
        //        return 0;
        UTILS_DIE("max_ploidy does not exist in the record!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<int8_t>>& VariantGenotype::getAlleles() const { return alleles; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<int8_t>>& VariantGenotype::getPhasing() const { return phasings; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::getNumberOfLikelihoods() const {
    if (!isLikelihoodPresent()) {
        return 0;
    } else {
        return static_cast<uint8_t>(likelihoods[0].size());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint32_t>>& VariantGenotype::getLikelihoods() const { return likelihoods; }

// ---------------------------------------------------------------------------------------------------------------------

bool VariantGenotype::getLinkedRecord() const { return static_cast<bool>(link_record); }

// ---------------------------------------------------------------------------------------------------------------------

const LinkRecord& VariantGenotype::getLinkRecord() const { return link_record.value(); }

 const std::vector<format_field>& VariantGenotype::getFormats() const { 
    return format; }

// ---------------------------------------------------------------------------------------------------------------------

// uint8_t VariantGenotype::getMaxPloidy() const {
//    if (isGenotypePresent()){
//        return alleles.front().size();
//    } else {
//        UTILS_DIE("max_ploidy does not exist in the record!");
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

format_field::format_field(util::BitReader& bitreader, uint32_t _sample_count) : sample_count(_sample_count) {
    format.resize(static_cast<uint8_t>(bitreader.read_b(8)));
    for (char& c : format) c = static_cast<char>(bitreader.read_b(8));
    type = static_cast<core::DataType>(bitreader.read_b(8));
    core::ArrayType arrayType;

    arrayLength = static_cast<uint8_t>(bitreader.read_b(8));
    value.resize(sample_count, std::vector<std::vector<uint8_t>>(arrayLength, std::vector<uint8_t>(0)));
    for (auto& formatArray : value) {
        for (auto& oneValue : formatArray) {
            auto temp = arrayType.toArray(type, bitreader);
            oneValue = temp;
            for (auto i = temp.size(); i > 0; --i) oneValue.at(i - 1) = temp.at(temp.size() - i);
        }
    }
}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */
#include <iostream>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

#include "genie/core/arrayType.h"
#include "genie/core/record/variant_genotype/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

VariantGenotype::VariantGenotype(uint64_t _variant_index, uint32_t _sample_index_from)

    : variant_index_(_variant_index),
      sample_index_from_(_sample_index_from),
      sample_count_(0),
      format_(),
      alleles_(),
      phasings_() {}

// -------------------------------------------------------------------------------------------------

VariantGenotype::VariantGenotype(util::BitReader& bitreader)
    : variant_index_(bitreader.ReadAlignedInt<uint64_t>()),
      sample_index_from_(bitreader.ReadAlignedInt<uint32_t>()),
      sample_count_(bitreader.ReadAlignedInt<uint32_t>()),
      format_(),
      alleles_(),
      phasings_() {
  if (!bitreader.IsStreamGood()) {
    return;
  }

  // std::cout << "format_count...";
  auto format_count = bitreader.ReadAlignedInt<uint8_t>();
  for (uint8_t i = 0; i < format_count; i++) {
    format_.emplace_back(bitreader, sample_count_);
  }

  bool genotype_present = bitreader.Read<bool>(8);
  bool likelihood_present = bitreader.Read<bool>(8);

  if (genotype_present) {
    // std::cout << "allele...";
    auto n_alleles_per_sample = bitreader.Read<uint8_t>(8);
    UTILS_DIE_IF(n_alleles_per_sample == 0, "Invalid n_alleles_per_sample!");

    alleles_.resize(sample_count_, std::vector<int8_t>(n_alleles_per_sample));
    if (n_alleles_per_sample > 1)
      phasings_.resize(sample_count_, std::vector<uint8_t>(n_alleles_per_sample - 1));

    for (auto& alleles_sample : alleles_) {
      for (auto& allele : alleles_sample) {
        // TODO (Yeremia): move this signed integer fix to btreader!
        allele = bitreader.ReadAlignedInt<int8_t>();
      }
    }
    // std::cout << "phasings...";
    if (n_alleles_per_sample - 1 > 0) {
      for (auto& phasings_sample : phasings_) {
        for (auto& phasing : phasings_sample) {
          phasing = bitreader.ReadAlignedInt<uint8_t>();
        }
      }
    }
  }

  if (likelihood_present) {
    // std::cout << "likelihood...";
    auto n_likelihoods = bitreader.ReadAlignedInt<uint8_t>();
    UTILS_DIE_IF(n_likelihoods == 0, "Invalid n_likelihoods!");

    likelihoods_.resize(sample_count_, std::vector<uint32_t>(n_likelihoods));

    for (auto& likelihood_sample : likelihoods_) {
      for (auto& likelihood : likelihood_sample) {
        likelihood = bitreader.ReadAlignedInt<uint32_t>();
      }
    }
  }

  auto linked_record = bitreader.Read<bool>(8);
  if (linked_record) {
    link_record_ = LinkRecord(bitreader);
  }
}

// -------------------------------------------------------------------------------------------------

uint64_t VariantGenotype::GetVariantIndex() const {
  return variant_index_;
}

// -------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::GetStartSampleIndex() const {
  return sample_index_from_;
}

// -------------------------------------------------------------------------------------------------

uint32_t VariantGenotype::GetNumSamples() const {
  return sample_count_;
}

// -------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::GetFormatCount() const {
  return static_cast<uint8_t>(format_.size());
}

// -------------------------------------------------------------------------------------------------

//
// std::vector<format_field> VariantGenotype::getFormat() const { return format; }
//

// -------------------------------------------------------------------------------------------------

bool VariantGenotype::IsGenotypePresent() const {
  return !alleles_.empty();
}

// -------------------------------------------------------------------------------------------------

bool VariantGenotype::IsLikelihoodPresent() const {
  return !likelihoods_.empty();
}

// -------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::GetNumberOfAllelesPerSample() const {
  if (IsGenotypePresent()) {
    return static_cast<uint8_t>(alleles_.front().size());
  } else {
    //        return 0;
    UTILS_DIE("max_ploidy_ does not exist in the record!");
  }
}

// -------------------------------------------------------------------------------------------------

const std::vector<std::vector<int8_t>>& VariantGenotype::GetAlleles() const {
  return alleles_;
}

// -------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint8_t>>& VariantGenotype::GetPhasing() const {
  return phasings_;
}

// -------------------------------------------------------------------------------------------------

uint8_t VariantGenotype::GetNumberOfLikelihoods() const {
  if (!IsLikelihoodPresent()) {
    return 0;
  } else {
    return static_cast<uint8_t>(likelihoods_[0].size());
  }
}

// -------------------------------------------------------------------------------------------------

const std::vector<std::vector<uint32_t>>& VariantGenotype::GetLikelihoods() const {
  return likelihoods_;
}

// -------------------------------------------------------------------------------------------------

bool VariantGenotype::GetLinkedRecord() const {
  return static_cast<bool>(link_record_);
}

// -------------------------------------------------------------------------------------------------

const LinkRecord& VariantGenotype::GetLinkRecord() const {
  return link_record_.value();
}

// -------------------------------------------------------------------------------------------------

const std::vector<FormatField>& VariantGenotype::GetFormats() const {
  return format_;
}

// -------------------------------------------------------------------------------------------------

// uint8_t VariantGenotype::GetMaxPloidy() const {
//    if (isGenotypePresent()){
//        return alleles.front().size();
//    } else {
//        UTILS_DIE("max_ploidy_ does not exist in the record!");
//    }
//}

// -------------------------------------------------------------------------------------------------

FormatField::FormatField(util::BitReader& bitreader, uint32_t sample_count)
    : sample_count_(sample_count) {
  format_.resize(bitreader.Read<uint8_t>());  // static_cast<uint8_t>(bitreader.ReadBits(8)));
  for (char& c : format_)
    c = static_cast<char>(bitreader.ReadBits(8));
  type_ = static_cast<core::DataType>(bitreader.ReadBits(8));
  core::ArrayType arrayType;

  array_length_ = bitreader.Read<uint8_t>();  // static_cast<uint8_t>(bitreader.ReadBits(8));
  value_.resize(sample_count,
               std::vector<std::vector<uint8_t>>(array_length_, std::vector<uint8_t>(0)));
  for (auto& formatArray : value_) {
    for (auto& oneValue : formatArray) {
      oneValue = arrayType.toArray(type_, bitreader);
      //   oneValue = temp;
      //   for (auto i = temp.size(); i > 0; --i) oneValue.at(i - 1) = temp.at(temp.size() - i);
    }
  }
}
// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

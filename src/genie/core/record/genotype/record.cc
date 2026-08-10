/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/genotype/record.h"

#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"
#include "genie/core/array_type.h"

// -----------------------------------------------------------------------------

namespace genie::core::record::genotype {

// -----------------------------------------------------------------------------

Record::Record() = default;

// -----------------------------------------------------------------------------

// Parameterized constructor
Record::Record(
    uint64_t variant_index,
    uint32_t sample_index_from,
    uint32_t sample_count,
    std::vector<FormatField>&& format,
    std::vector<std::vector<int8_t>>&& alleles,
    std::vector<std::vector<uint8_t>>&& phasings,
    std::vector<std::vector<uint32_t>>&& likelihoods,
    const std::optional<linked::Record>& link_record)
    : variant_index_(variant_index),
      sample_index_from_(sample_index_from),
      sample_count_(sample_count),
      format_(std::move(format)),
      alleles_(std::move(alleles)),
      phasings_(std::move(phasings)),
      likelihoods_(std::move(likelihoods)),
      link_record_(link_record) {
}

// -----------------------------------------------------------------------------

// Record::Record(uint64_t _variant_index, uint32_t _sample_index_from)
//
//    : variant_index(_variant_index),
//      sample_index_from(_sample_index_from),
//      sample_count(0),
//      format(),
//      alleles(),
//      phasings() {}

// -----------------------------------------------------------------------------

// Copy constructor
Record::Record(const Record& other) {
  variant_index_ = other.variant_index_;
  sample_index_from_ = other.sample_index_from_;
  sample_count_ = other.sample_count_;
  format_ = other.format_;
  alleles_ = other.alleles_;
  phasings_ = other.phasings_;
  likelihoods_ = other.likelihoods_;
  link_record_ = other.link_record_;
}

// -----------------------------------------------------------------------------

// Move constructor
Record::Record(Record&& other) noexcept {
  variant_index_ = other.variant_index_;
  sample_index_from_ = other.sample_index_from_;
  sample_count_ = other.sample_count_;
  format_ = std::move(other.format_);
  alleles_ = std::move(other.alleles_);
  phasings_ = std::move(other.phasings_);
  likelihoods_ = std::move(other.likelihoods_);
  link_record_ = std::move(other.link_record_);
}

// -----------------------------------------------------------------------------

// Copy assignment operator
Record& Record::operator=(const Record& other) {
  if (this != &other) {
    variant_index_ = other.variant_index_;
    sample_index_from_ = other.sample_index_from_;
    sample_count_ = other.sample_count_;
    format_ = other.format_;
    alleles_ = other.alleles_;
    phasings_ = other.phasings_;
    likelihoods_ = other.likelihoods_;
    link_record_ = other.link_record_;
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Move assignment operator
Record& Record::operator=(Record&& other) noexcept {
  if (this != &other) {
    variant_index_ = other.variant_index_;
    sample_index_from_ = other.sample_index_from_;
    sample_count_ = other.sample_count_;
    format_ = std::move(other.format_);
    alleles_ = std::move(other.alleles_);
    phasings_ = std::move(other.phasings_);
    likelihoods_ = std::move(other.likelihoods_);
    link_record_ = std::move(other.link_record_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

Record::Record(util::BitReader& reader)
    : variant_index_(reader.ReadAlignedInt<uint64_t>()),
      sample_index_from_(reader.ReadAlignedInt<uint32_t>()),
      sample_count_(reader.ReadAlignedInt<uint32_t>()),
      format_(),
      alleles_(),
      phasings_() {

    if (!reader.IsStreamGood()) {
        return;
    }

    // std::cout << "format_count...";
    auto format_count = reader.ReadAlignedInt<uint8_t>();
    for (uint8_t idx_i = 0; idx_i < format_count; idx_i++) {
        format_.emplace_back(reader, sample_count_);
    }

    bool genotype_present = reader.Read<bool>(8);
    bool likelihood_present = reader.Read<bool>(8);

    if (genotype_present) {
        // std::cout << "allele...";
        auto n_alleles_per_sample = reader.Read<uint8_t>(8);
        UTILS_DIE_IF(n_alleles_per_sample == 0, "Invalid n_alleles_per_sample!");

        alleles_.resize(sample_count_, std::vector<int8_t>(n_alleles_per_sample));
        if (n_alleles_per_sample > 1) phasings_.resize(sample_count_, std::vector<uint8_t>(n_alleles_per_sample - 1));

        for (auto& alleles_sample : alleles_) {
            for (auto& allele : alleles_sample) {
                // TODO(Yeremia): move this signed integer fix to btreader!
                allele = reader.ReadAlignedInt<int8_t>();
            }
        }
        // std::cout << "phasings...";
        if (n_alleles_per_sample - 1 > 0) {
            for (auto& phasings_sample : phasings_) {
                for (auto& phasing : phasings_sample) {
                    phasing = reader.ReadAlignedInt<uint8_t>();
                }
            }
        }
    }

    if (likelihood_present) {
       // std::cout << "likelihood...";
        auto n_likelihoods = reader.ReadAlignedInt<uint8_t>();
        UTILS_DIE_IF(n_likelihoods == 0, "Invalid n_likelihoods!");

        likelihoods_.resize(sample_count_, std::vector<uint32_t>(n_likelihoods));

        for (auto& likelihood_sample : likelihoods_) {
            for (auto& likelihood : likelihood_sample) {
                likelihood = reader.ReadAlignedInt<uint32_t>();
            }
        }
    }

    auto linked_record = reader.Read<bool>(8);
    if (linked_record) {
        link_record_ = linked::Record(reader);
    }
}

uint64_t Record::GetVariantIndex() const { return variant_index_; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Record::GetSampleIndexFrom() const { return sample_index_from_; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Record::GetSampleCount() const { return sample_count_; }

// -----------------------------------------------------------------------------

const std::vector<FormatField>& Record::GetFormat() const { return format_; }

// -----------------------------------------------------------------------------

const std::vector<std::vector<int8_t>>& Record::GetAlleles() const { return alleles_; }

// -----------------------------------------------------------------------------

const std::vector<std::vector<uint8_t>>& Record::GetPhasing() const { return phasings_; }

// -----------------------------------------------------------------------------

const std::vector<std::vector<uint32_t>>& Record::GetLikelihoods() const { return likelihoods_; }

// -----------------------------------------------------------------------------

const std::optional<linked::Record>& Record::GetLinkRecord() const { return link_record_; }

// -----------------------------------------------------------------------------

void Record::SetVariantIndex(uint64_t value) { variant_index_ = value; }

// -----------------------------------------------------------------------------

void Record::SetSampleIndexFrom(uint32_t value) { sample_index_from_ = value; }

// -----------------------------------------------------------------------------

void Record::SetSampleCount(uint32_t value) { sample_count_ = value; }

// -----------------------------------------------------------------------------

void Record::SetFormat(std::vector<FormatField> value) { format_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetFormat(std::vector<FormatField>&& value) { format_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetAlleles(std::vector<std::vector<int8_t>> value) { alleles_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetAlleles(std::vector<std::vector<int8_t>>&& value) { alleles_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetPhasings(std::vector<std::vector<uint8_t>> value) { phasings_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetPhasings(std::vector<std::vector<uint8_t>>&& value) { phasings_ = std::move(value); }


// -----------------------------------------------------------------------------

void Record::SetLikelihoods(std::vector<std::vector<uint32_t>> value) { likelihoods_ = std::move(value); }

// -----------------------------------------------------------------------------

void Record::SetNumberOfLikelihoods(uint8_t value) {
    if (likelihoods_.size() != sample_count_) {
        likelihoods_.resize(sample_count_);
    }
    for (auto& l : likelihoods_) {
        l.resize(value);
    }
}

// -----------------------------------------------------------------------------

void Record::SetLinkRecord(const std::optional<linked::Record>& value) { link_record_ = value; }

// -----------------------------------------------------------------------------

uint8_t Record::GetFormatCount() const { return static_cast<uint8_t>(format_.size()); }

// -----------------------------------------------------------------------------

bool Record::IsGenotypePresent() const { return !alleles_.empty(); }

// -----------------------------------------------------------------------------

uint8_t Record::GetNumberOfAllelesPerSample() const {
  if (IsGenotypePresent()) {
    return static_cast<uint8_t>(alleles_.front().size());
  } else {
    //        return 0;
    UTILS_DIE("max_ploidy_ does not exist in the record!");
  }
}

// -----------------------------------------------------------------------------

bool Record::IsLikelihoodPresent() const { return !likelihoods_.empty(); }

// -----------------------------------------------------------------------------

uint8_t Record::GetNumberOfLikelihoods() const {
    if (!IsLikelihoodPresent()) {
        return 0;
    } else {
        return static_cast<uint8_t>(likelihoods_[0].size());
    }
}

// -----------------------------------------------------------------------------

bool Record::GetLinkedRecord() const { return link_record_.has_value(); }

// -----------------------------------------------------------------------------

// Size calculation
size_t Record::GetSize() const {
//  size_t size = 0;
//  size += sizeof(variant_index_);
//  size += sizeof(sample_index_from_);
//  size += sizeof(sample_count_);
//
//  size += format_.size() * sizeof(FormatField);
//
//  for (const auto& allele_vector : alleles_) {
//    size += allele_vector.size() * sizeof(int8_t);
//  }
//
//  for (const auto& phasing_vector : phasings_) {
//    size += phasing_vector.size() * sizeof(uint8_t);
//  }
//
//  for (const auto& likelihood_vector : likelihoods_) {
//    size += likelihood_vector.size() * sizeof(uint32_t);
//  }
//
//  if (link_record_.has_value()) {
//    size += sizeof(LinkRecord);
//  }
//
//  return size;
    // TODO(Yeremia): Implement
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

FormatField::FormatField(util::BitReader& bitreader, uint32_t _sample_count) : sample_count_(_sample_count) {
    format_.resize(bitreader.Read<uint8_t>());  // static_cast<uint8_t>(bitreader.ReadBits(8)));
    for (char& c : format_) c = static_cast<char>(bitreader.ReadBits(8));
    type_ = static_cast<core::DataType>(bitreader.ReadBits(8));
    core::ArrayType arrayType;

    array_length_ = bitreader.Read<uint8_t>();  // static_cast<uint8_t>(bitreader.ReadBits(8));
    value_.resize(sample_count_, std::vector<std::vector<uint8_t>>(array_length_, std::vector<uint8_t>(0)));
    for (auto& formatArray : value_) {
        for (auto& oneValue : formatArray) {
            oneValue = arrayType.toArray(type_, bitreader);
            //   oneValue = temp;
            //   for (auto idx_i = temp.size(); idx_i > 0; --idx_i) oneValue.at(idx_i - 1) = temp.at(temp.size() - idx_i);
        }
    }
}
// ---------------------------------------------------------------------------------------------------------------------
bool operator==(const FormatField& lhs, const FormatField& rhs) {
    return lhs.GetFormat() == rhs.GetFormat() &&
           lhs.GetType() == rhs.GetType() &&
           lhs.GetSampleCount() == rhs.GetSampleCount() &&
           lhs.GetValue() == rhs.GetValue() &&
           lhs.GetArrayLength() == rhs.GetArrayLength();
}

bool operator==(const Record& lhs, const Record& rhs) {
    return lhs.GetVariantIndex() == rhs.GetVariantIndex() &&
           lhs.GetSampleIndexFrom() == rhs.GetSampleIndexFrom() &&
           lhs.GetSampleCount() == rhs.GetSampleCount() &&
           lhs.GetFormat() == rhs.GetFormat() &&
           lhs.GetAlleles() == rhs.GetAlleles() &&
           lhs.GetPhasing() == rhs.GetPhasing() &&
           lhs.GetLikelihoods() == rhs.GetLikelihoods() &&
           lhs.GetLinkRecord() == rhs.GetLinkRecord();
}

}  // namespace genie::core::record::genotype

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

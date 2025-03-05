/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_payload.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

#include "genotype_coder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

// Default constructor
GenotypePayload::GenotypePayload() {
  max_ploidy_ = 0;
  no_reference_flag_ = false;
  not_available_flag_ = false;
  phases_value_ = false;
}

// -----------------------------------------------------------------------------

// Parameterized constructor
GenotypePayload::GenotypePayload(
    uint8_t max_ploidy,
    bool no_reference_flag,
    bool not_available_flag,
    bool phases_value,
    std::vector<SortedBinMatPayload>&& variants_payload,
    std::optional<AmaxPayload>&& variants_amax_payload,
    std::optional<SortedBinMatPayload>&& phases_payload)
    : max_ploidy_(max_ploidy),
      no_reference_flag_(no_reference_flag),
      not_available_flag_(not_available_flag),
      phases_value_(phases_value),
      variants_payloads_(std::move(variants_payload)),
      variants_amax_payload_(std::move(variants_amax_payload)),
      phases_payload_(std::move(phases_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

// Copy constructor
//GenotypePayload::GenotypePayload(const GenotypePayload& other) = default;

// ---------------------------------------------------------------------------------------------------------------------

// Move constructor
GenotypePayload::GenotypePayload(GenotypePayload&& other) noexcept {
  max_ploidy_ = other.max_ploidy_;
  no_reference_flag_ = other.no_reference_flag_;
  not_available_flag_ = other.not_available_flag_;
  phases_value_ = other.phases_value_;
  variants_payloads_ = std::move(other.variants_payloads_);
  variants_amax_payload_ = std::move(other.variants_amax_payload_);
  phases_payload_ = std::move(other.phases_payload_);
}

// ---------------------------------------------------------------------------------------------------------------------

// Copy assignment operator
//GenotypePayload& GenotypePayload::operator=(const GenotypePayload& other) {
//  if (this != &other) {
//    max_ploidy_ = other.max_ploidy_;
//    no_reference_flag_ = other.no_reference_flag_;
//    not_available_flag_ = other.not_available_flag_;
//    phases_value_ = other.phases_value_;
//    variants_payloads_ = other.variants_payloads_;
//    sort_variants_row_ids_payloads_ = other.sort_variants_row_ids_payloads_;
//    sort_variants_col_ids_payloads_ = other.sort_variants_col_ids_payloads_;
//    variants_amax_payload_ = other.variants_amax_payload_;
//    phases_payload_ = other.phases_payload_;
//  }
//  return *this;
//}

// -----------------------------------------------------------------------------

// Move assignment operator
GenotypePayload& GenotypePayload::operator=(GenotypePayload&& other) noexcept {
  if (this != &other) {
    max_ploidy_ = other.max_ploidy_;
    no_reference_flag_ = other.no_reference_flag_;
    not_available_flag_ = other.not_available_flag_;
    phases_value_ = other.phases_value_;
    variants_payloads_ = std::move(other.variants_payloads_);
    variants_amax_payload_ = std::move(other.variants_amax_payload_);
    phases_payload_ = std::move(other.phases_payload_);
  }
  return *this;
}

// -----------------------------------------------------------------------------

// Constructor from BitReader
GenotypePayload::GenotypePayload(
  util::BitReader& reader,
  GenotypeParameters& parameters
) {

  UTILS_DIE_IF(!reader.IsByteAligned(), "Not byte aligned!");
  max_ploidy_ = reader.Read<uint8_t>();

  // reserved u(5) is not used, skip it
  auto flags = reader.Read<uint8_t>();
  no_reference_flag_ = (flags >> 2) & 1;
  not_available_flag_ = (flags >> 1) & 1;
  phases_value_ = (flags >> 0) & 1;

  auto num_bit_planes = reader.Read<uint8_t>();
  for (auto i=0u; i< num_bit_planes; i++){
    variants_payloads_.emplace_back(
      reader,
      parameters.GetAllelesCodecID(),
      parameters.GetSortAllelesRowsFlag(),
      parameters.GetSortAllelesColsFlag()
    );
  }

  if (parameters.GetBinarizationID() == BinarizationID::ROW_BIN){
    auto amax_payload_size = reader.Read<uint32_t>();
    variants_amax_payload_ = AmaxPayload(reader);
  }

  if (parameters.GetEncodePhasesDataFlag()){
    phases_payload_ = SortedBinMatPayload(
      reader,
        parameters.GetPhasesCodecID(),
        parameters.GetSortPhasesRowsFlag(),
        parameters.GetSortPhasesColsFlag()
    );
  }
}

// -----------------------------------------------------------------------------

[[maybe_unused]] [[maybe_unused]] [[maybe_unused]] uint8_t GenotypePayload::GetMaxPloidy() const {
  return max_ploidy_;
}

// -----------------------------------------------------------------------------

bool GenotypePayload::GetNoReferenceFlag() const {
  return no_reference_flag_;
}

// -----------------------------------------------------------------------------

bool GenotypePayload::GetNotAvailableFlag() const {
  return not_available_flag_;
}

// -----------------------------------------------------------------------------

bool GenotypePayload::GetPhasesValue() const {
  return phases_value_;
}

// -----------------------------------------------------------------------------

uint8_t GenotypePayload::GetNumBitPlanes() const { return static_cast<uint8_t>(GetVariantsPayloads().size()); }

// -----------------------------------------------------------------------------

const std::vector<SortedBinMatPayload>& GenotypePayload::GetVariantsPayloads() const {
  return variants_payloads_;
}

// -----------------------------------------------------------------------------

bool GenotypePayload::IsAmaxPayloadExist() const { return variants_amax_payload_.has_value(); }

// -----------------------------------------------------------------------------

const std::optional<AmaxPayload>& GenotypePayload::GetVariantsAmaxPayload() const {
  return variants_amax_payload_;
}

// -----------------------------------------------------------------------------

bool GenotypePayload::EncodePhaseValues() const { return GetPhasesPayload().has_value(); }

// -----------------------------------------------------------------------------

const std::optional<SortedBinMatPayload>& GenotypePayload::GetPhasesPayload() const {
  return phases_payload_;
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetMaxPloidy(uint8_t max_ploidy) {
  max_ploidy_ = max_ploidy;
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetNoReferenceFlag(bool no_reference_flag) {
  no_reference_flag_ = no_reference_flag;
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetNotAvailableFlag(bool not_available_flag) {
  not_available_flag_ = not_available_flag;
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetPhasesValue(bool phases_value) {
  phases_value_ = phases_value;
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetVariantsPayloads(std::vector<SortedBinMatPayload>&& variants_payload) {
  variants_payloads_ = std::move(variants_payload);
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetVariantsAmaxPayload(std::optional<AmaxPayload>&& variants_amax_payload) {
  variants_amax_payload_ = std::move(variants_amax_payload);
}

// -----------------------------------------------------------------------------

void GenotypePayload::SetPhasesPayload(std::optional<SortedBinMatPayload>&& phases_payload) {
  phases_payload_ = std::move(phases_payload);
}

// -----------------------------------------------------------------------------

// Size calculation
size_t GenotypePayload::GetSize() const {
  size_t size = 0;
  size += sizeof(max_ploidy_);
  size += sizeof(uint8_t); // num_bit_planes
  size += sizeof(uint8_t); // no_reference_flag, not_available_flag_, phases_value_
  for (auto& variant_payload: variants_payloads_){
    size += variant_payload.GetSize();
  }

  if (IsAmaxPayloadExist()) {
    size += sizeof(uint32_t);
    size += GetVariantsAmaxPayload()->GetSize();
  }

  if (EncodePhaseValues()) {
    size += GetPhasesPayload()->GetSize();
  }
  return size;
}

// -----------------------------------------------------------------------------

void GenotypePayload::Write(util::BitWriter& writer) const {
  UTILS_DIE_IF(!writer.IsByteAligned(), "Not byte aligned!");
  writer.WriteBypassBE(GetMaxPloidy());

  uint8_t flag = 0;
  flag |= GetNoReferenceFlag() << 2;
  flag |= GetNotAvailableFlag() << 1;
  flag |= GetPhasesValue() << 0;
  writer.WriteBypassBE(flag);

  writer.WriteBypassBE(GetNumBitPlanes());
  for (const auto& variant_payload: GetVariantsPayloads()){
    variant_payload.Write(writer);
  }

  if (IsAmaxPayloadExist()) {
    writer.WriteBypassBE(static_cast<uint32_t>(GetVariantsAmaxPayload()->GetSize()));
    GetVariantsAmaxPayload()->Write(writer);
  }

  if (EncodePhaseValues()) {
    GetPhasesPayload()->Write(writer);
  }
}

// -----------------------------------------------------------------------------

void GenotypePayload::Write(core::Writer& writer) const {

}

// -----------------------------------------------------------------------------

//GenotypePayload::GenotypePayload(EncodingBlock& datablock, GenotypeParameters& genotypeParameters)
//    : genotype_parameters_(genotypeParameters) {
//
//    // Initialize phases payload
//    phases_payload_ = BinMatPayload(datablock.phasing_mat, genotypeParameters.GetPhasesCodecID());
//
//    // Initialize variants payloads
//    for (auto & allele_bin_mat : datablock.allele_bin_mat_vect) {
//      variants_payloads_.emplace_back(
//            allele_bin_mat,
//            genotypeParameters.GetAllelesCodecID()
//        );
//    }
//
//    // Initialize row IDs payloads
//    for (size_t i = 0; i < datablock.allele_row_ids_vect.size(); ++i) {
//      auto& alleleRowIDs = datablock.allele_row_ids_vect[i];
//      auto shape = datablock.allele_bin_mat_vect[i].shape();
//      auto nrows = static_cast<uint32_t>(shape[0]);
//      auto nBitsPerElem = static_cast<uint32_t>(std::ceil(std::log2(nrows)));
//
//      std::vector<uint64_t> payloadVec(alleleRowIDs.begin(), alleleRowIDs.end());
//      sort_variants_row_ids_payloads_.emplace_back(std::move(payloadVec));
//    }
//
//    // Initialize column IDs payloads
//    for (size_t i = 0; i < datablock.allele_col_ids_vect.size(); ++i) {
//      auto& alleleColIDs = datablock.allele_col_ids_vect[i];
//      auto shape = datablock.allele_bin_mat_vect[i].shape();
//      auto ncols = static_cast<uint32_t>(shape[1]);
//      auto nBitsPerElem = static_cast<uint32_t>(std::ceil(std::log2(ncols)));
//
//      std::vector<uint64_t> payloadVec(alleleColIDs.begin(), alleleColIDs.end());
//      sort_variants_col_ids_payloads_.emplace_back(std::move(payloadVec));
//    }
//
//    // Initialize amax payload if binarization is ROW_BIN
//    if (genotypeParameters.GetBinarizationID() == BinarizationID::ROW_BIN) {
//      std::vector<uint64_t> amax_elements;
//      for (auto amax : datablock.amax_vec) {
//        amax_elements.push_back(amax);
//      }
//
//      variants_amax_payload_ = AmaxPayload(std::move(amax_elements));
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

//void genie::genotype::GenotypePayload::Write(core::Writer& writer) const {
//    size_t indecRowIds = 0;
//    size_t indecColIds = 0;
//    uint8_t num_variants_payloads =
//        genotype_parameters_.GetBinarizationID() == BinarizationID::BIT_PLANE &&
//        genotype_parameters_.GetConcatAxis() == ConcatAxis::DO_NOT_CONCAT
//        ? genotype_parameters_.GetNumBitPlanes()
//        : 1;
//
//    for (auto i = 0; i < num_variants_payloads; ++i) {
//        std::stringstream tempstream;
//        core::Writer writesize(&tempstream);
//        variants_payloads_[i].WriteCompressed(writesize);
//        auto variantssize = tempstream.str().size();
//        writer.Write(variantssize, 32);
//        variants_payloads_[i].WriteCompressed(writer);
//        auto variantsPayloadsParams =
//            genotype_parameters_.GsetVariantsPayloadParams();
//        if (variantsPayloadsParams[i].sort_rows_flag) {
//            uint32_t size = sort_variants_row_ids_payload[indecRowIds].GetSize();
//            writer.Write(size, 32);
//            sort_variants_row_ids_payload[indecRowIds].Write(writer);
//            ++indecRowIds;
//        }
//        if (variantsPayloadsParams[i].sort_cols_flag) {
//            auto size = sort_variants_col_ids_payload[indecColIds].GetSize();
//            writer.Write(size, 32);
//            sort_variants_col_ids_payload[indecColIds].Write(writer);
//            ++indecColIds;
//        }
//        if (genotype_parameters_.GetBinarizationID() == BinarizationID::ROW_BIN) {
//          writer.Write(variants_amax_payload_.GetSize(), 32);
//          variants_amax_payload_.Write(writer);
//        }
//    }
//    if (genotype_parameters_.IsPhaseEncoded()) {
//        std::stringstream tempstream;
//        core::Writer writesize(&tempstream);
//        phases_payload_.WriteCompressed(writesize);
//        auto phasesSize = tempstream.str().size();
//        writer.Write(phasesSize, 32);
//        phases_payload_.WriteCompressed(writer);
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------
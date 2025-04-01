/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genotype_parameters.h"

#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters()
    : binarization_ID_(BinarizationID::UNDEFINED),
      concat_axis_(ConcatAxis::UNDEFINED),
      transpose_alleles_mat_flag_(false),
      sort_alleles_rows_flag_(false),
      sort_alleles_cols_flag_(false),
      alleles_codec_ID_(genie::core::AlgoID::JBIG),
      encode_phases_data_flag_(false),
      transpose_phases_mat_flag_(false),
      sort_phases_rows_flag_(false),
      sort_phases_cols_flag_(false),
      phases_codec_ID_(genie::core::AlgoID::JBIG) {}

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters(
    BinarizationID binarization_id,
    ConcatAxis concat_axis,
    bool transpose_alleles_mat_flag,
    bool sort_alleles_rows_flag,
    bool sort_alleles_cols_flag,
    genie::core::AlgoID alleles_codec_id,
    bool encode_phases_data_flag,
    bool transpose_phases_mat_flag,
    bool sort_phases_rows_flag,
    bool sort_phases_cols_flag,
    genie::core::AlgoID phases_codec_id)
    : binarization_ID_(binarization_id),
      concat_axis_(concat_axis),
      transpose_alleles_mat_flag_(transpose_alleles_mat_flag),
      sort_alleles_rows_flag_(sort_alleles_rows_flag),
      sort_alleles_cols_flag_(sort_alleles_cols_flag),
      alleles_codec_ID_(alleles_codec_id),
      encode_phases_data_flag_(encode_phases_data_flag),
      transpose_phases_mat_flag_(transpose_phases_mat_flag),
      sort_phases_rows_flag_(sort_phases_rows_flag),
      sort_phases_cols_flag_(sort_phases_cols_flag),
      phases_codec_ID_(phases_codec_id) {}

// ---------------------------------------------------------------------------------------------------------------------

// Copy constructor
GenotypeParameters::GenotypeParameters(const GenotypeParameters& other) = default;

// ---------------------------------------------------------------------------------------------------------------------

// Move constructor
GenotypeParameters::GenotypeParameters(GenotypeParameters&& other) noexcept = default;

// ---------------------------------------------------------------------------------------------------------------------

// Copy assignment operator
GenotypeParameters& GenotypeParameters::operator=(const GenotypeParameters& other) = default;

// ---------------------------------------------------------------------------------------------------------------------

// Move assignment operator
GenotypeParameters& GenotypeParameters::operator=(GenotypeParameters&& other) noexcept = default;

// ---------------------------------------------------------------------------------------------------------------------

GenotypeParameters::GenotypeParameters(
    genie::util::BitReader& reader
) {
  UTILS_DIE_IF(!reader.IsByteAligned(), "Not byte aligned!");

  reader.ReadBits(3); // reserved u(3)
  binarization_ID_ = static_cast<BinarizationID>(reader.ReadBits(3));
  concat_axis_ = static_cast<ConcatAxis>(reader.ReadBits(2));

  reader.ReadBits(2); // reserved u(2)
  transpose_alleles_mat_flag_ = reader.ReadBits(1);
  sort_alleles_rows_flag_ = reader.ReadBits(1);
  sort_alleles_cols_flag_ = reader.ReadBits(1);
  alleles_codec_ID_ = static_cast<genie::core::AlgoID>(reader.ReadBits(3));

  reader.ReadBits(1); // reserved u(1)
  encode_phases_data_flag_ = reader.ReadBits(1);
  transpose_phases_mat_flag_ = reader.ReadBits(1);
  sort_phases_rows_flag_ = reader.ReadBits(1);
  sort_phases_cols_flag_ = reader.ReadBits(1);
  phases_codec_ID_ = static_cast<genie::core::AlgoID>(reader.ReadBits(3));
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] BinarizationID GenotypeParameters::GetBinarizationID() const {
  return binarization_ID_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] ConcatAxis GenotypeParameters::GetConcatAxis() const {
  return concat_axis_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetTransposeAllelesMatFlag() const {
  return transpose_alleles_mat_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetSortAllelesRowsFlag() const {
  return sort_alleles_rows_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetSortAllelesColsFlag() const {
  return sort_alleles_cols_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::AlgoID GenotypeParameters::GetAllelesCodecID() const {
  return alleles_codec_ID_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool GenotypeParameters::GetEncodePhasesDataFlag() const {
  return encode_phases_data_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] bool GenotypeParameters::GetTransposePhasesMatFlag() const {
  return transpose_phases_mat_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] bool GenotypeParameters::GetSortPhasesRowsFlag() const {
  return sort_phases_rows_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] bool GenotypeParameters::GetSortPhasesColsFlag() const {
  return sort_phases_cols_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] genie::core::AlgoID GenotypeParameters::GetPhasesCodecID() const {
  return phases_codec_ID_;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetBinarizationID(BinarizationID binarization_id) {
  binarization_ID_ = binarization_id;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetConcatAxis(ConcatAxis concat_axis) {
  concat_axis_ = concat_axis;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetTransposeAllelesMatFlag(bool flag) {
  transpose_alleles_mat_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetSortAllelesRowsFlag(bool flag) {
  sort_alleles_rows_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetSortAllelesColsFlag(bool flag) {
  sort_alleles_cols_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetAllelesCodecID(genie::core::AlgoID codec_id) {
  alleles_codec_ID_ = codec_id;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetEncodePhasesDataFlag(bool flag) {
  encode_phases_data_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetTransposePhasesMatFlag(bool flag) {
  transpose_phases_mat_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetSortPhasesRowsFlag(bool flag) {
  sort_phases_rows_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void GenotypeParameters::SetSortPhasesColsFlag(bool flag) {
  sort_phases_cols_flag_ = flag;
}

// ---------------------------------------------------------------------------------------------------------------------

void GenotypeParameters::SetPhasesCodecID(genie::core::AlgoID codec_id) {
    phases_codec_ID_ = codec_id;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t GenotypeParameters::GetSize() {
  size_t size = 0u;

  size+= 3; // reserved(3)
  size+= 3; // binarization_ID(3)
  size+= 2; // concat_axis(2)

  size+= 2; // reserved(2)
  size+= 1; // transpose_alleles_mat_flag_(1)
  size+= 1; // sort_alleles_rows_flag_(1)
  size+= 1; // sort_alleles_cols_flag_(1)
  size+= 3; // alleles_codec_ID(3)

  size+= 1; // reserved(1)
  size+= 1; // encode_phase_data_flag(1)
  size+= 1; // transpose_phases_mat_flag_(1)
  size+= 1; // sort_phases_rows_flag_(1)
  size+= 1; // sort_phases_cols_flag_(1)
  size+= 3; // phases_codec_ID(3)

  size <<= 3; // Bits to bytes
  return size;
}

// ---------------------------------------------------------------------------------------------------------------------

void GenotypeParameters::Write(util::BitWriter& writer) const {

  UTILS_DIE_IF(!writer.IsByteAligned(), "Not byte aligned!");

  writer.WriteBits(0u, 3); // reserved(3)
  writer.WriteBits(static_cast<uint64_t>(binarization_ID_), 3);
  writer.WriteBits(static_cast<uint64_t>(concat_axis_), 2);

  writer.WriteBits(0u, 2); // reserved(2)
  writer.WriteBits(GetTransposeAllelesMatFlag(), 1);
  writer.WriteBits(GetSortAllelesRowsFlag(), 1);
  writer.WriteBits(GetSortAllelesColsFlag(), 1);
  writer.WriteBits(static_cast<uint64_t>(GetAllelesCodecID()), 3);

  writer.WriteBits(0u, 1); // reserved(1)
  writer.WriteBits(GetEncodePhasesDataFlag(), 1);
  writer.WriteBits(GetTransposePhasesMatFlag(), 1);
  writer.WriteBits(GetSortPhasesRowsFlag(), 1);
  writer.WriteBits(GetSortPhasesColsFlag(), 1);
  writer.WriteBits(static_cast<uint64_t>(GetPhasesCodecID()), 3);
}

// ---------------------------------------------------------------------------------------------------------------------

void GenotypeParameters::Write(core::Writer& writer) const {
  writer.Write(0u, 3); // reserved(3)
  writer.Write(static_cast<uint64_t>(binarization_ID_), 3);
  writer.Write(static_cast<uint64_t>(concat_axis_), 2);

  writer.Write(0u, 2); // reserved(2)
  writer.Write(GetTransposeAllelesMatFlag(), 1);
  writer.Write(GetSortAllelesRowsFlag(), 1);
  writer.Write(GetSortAllelesColsFlag(), 1);
  writer.Write(static_cast<uint64_t>(GetAllelesCodecID()), 3);

  writer.Write(0u, 1); // reserved(1)
  writer.Write(GetEncodePhasesDataFlag(), 1);
  writer.Write(GetTransposePhasesMatFlag(), 1);
  writer.Write(GetSortPhasesRowsFlag(), 1);
  writer.Write(GetSortPhasesColsFlag(), 1);
  writer.Write(static_cast<uint64_t>(GetPhasesCodecID()), 3);
  writer.Flush();
}

// ---------------------------------------------------------------------------------------------------------------------

// Read from BitReader
void GenotypeParameters::read(util::BitReader& reader) {
  UTILS_DIE_IF(!reader.IsByteAligned(), "Not byte aligned!");

  reader.ReadBits(3);  // reserved u(3)
  binarization_ID_ = static_cast<BinarizationID>(reader.ReadBits(3));
  concat_axis_ = static_cast<ConcatAxis>(reader.ReadBits(2));

  reader.ReadBits(2);  // reserved u(2)
  transpose_alleles_mat_flag_ = reader.ReadBits(1);
  sort_alleles_rows_flag_ = reader.ReadBits(1);
  sort_alleles_cols_flag_ = reader.ReadBits(1);
  alleles_codec_ID_ = static_cast<genie::core::AlgoID>(reader.ReadBits(3));

  reader.ReadBits(1);  // reserved u(1)
  encode_phases_data_flag_ = reader.ReadBits(1);
  transpose_phases_mat_flag_ = reader.ReadBits(1);
  sort_phases_rows_flag_ = reader.ReadBits(1);
  sort_phases_cols_flag_ = reader.ReadBits(1);
  phases_codec_ID_ = static_cast<genie::core::AlgoID>(reader.ReadBits(3));
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------
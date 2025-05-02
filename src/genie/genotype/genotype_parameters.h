/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PARAMETERS_H
#define GENIE_GENOTYPE_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/core/writer.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie::genotype {

// ---------------------------------------------------------------------------------------------------------------------

enum class BinarizationID : uint8_t {
    BIT_PLANE = 0,
    ROW_BIN = 1,
    UNDEFINED = 2,
};

// ---------------------------------------------------------------------------------------------------------------------

enum class ConcatAxis : uint8_t {
    CONCAT_ROW_DIR = 0,
    CONCAT_COL_DIR = 1,
    DO_NOT_CONCAT = 2,
    UNDEFINED = 3,
};

// ---------------------------------------------------------------------------------------------------------------------

class GenotypeParameters {
 private:
  BinarizationID binarization_ID_;
  ConcatAxis concat_axis_;
  bool sort_variants_rows_flag_;
  bool sort_variants_cols_flag_;
  bool transpose_variants_mat_flag_;
  genie::core::AlgoID variants_codec_ID_;
  bool encode_phases_data_flag_;
  bool sort_phases_rows_flag_;
  bool sort_phases_cols_flag_;
  bool transpose_phases_mat_flag_;
  genie::core::AlgoID phases_codec_ID_;

 public:
  // Default constructor
  GenotypeParameters();

  // Constructor with all variables
  GenotypeParameters(
      BinarizationID binarization_id,
      ConcatAxis concat_axis,
      bool sort_variants_rows_flag,
      bool sort_variants_cols_flag,
      bool transpose_variants_mat_flag,
      genie::core::AlgoID variants_codec_id,
      bool encode_phases_data_flag,
      bool sort_phases_rows_flag,
      bool sort_phases_cols_flag,
      bool transpose_phases_mat_flag,
      genie::core::AlgoID phases_codec_id
  );

  // Copy constructor
  GenotypeParameters(const GenotypeParameters& other);

  // Move constructor
  GenotypeParameters(GenotypeParameters&& other) noexcept;

  // Copy assignment operator
  GenotypeParameters& operator=(const GenotypeParameters& other);

  // Move assignment operator
  GenotypeParameters& operator=(GenotypeParameters&& other) noexcept;

  // Constructor from bitreader
  explicit GenotypeParameters(genie::util::BitReader& reader);

  // Getters
  [[maybe_unused]] BinarizationID GetBinarizationID() const;
  [[maybe_unused]] ConcatAxis GetConcatAxis() const;
  bool GetTransposeVariantsMatFlag() const;
  bool GetSortVariantsRowsFlag() const;
  bool GetSortVariantsColsFlag() const;
  genie::core::AlgoID GetVariantsCodecID() const;
  bool GetEncodePhasesDataFlag() const;
  [[maybe_unused]] bool GetTransposePhasesMatFlag() const;
  [[maybe_unused]] bool GetSortPhasesRowsFlag() const;
  [[maybe_unused]] bool GetSortPhasesColsFlag() const;
  [[maybe_unused]] genie::core::AlgoID GetPhasesCodecID() const;

  // Setters
  [[maybe_unused]] void SetBinarizationID(BinarizationID binarization_id);
  [[maybe_unused]] void SetConcatAxis(ConcatAxis concat_axis);
  [[maybe_unused]] void SetTransposeVariantsMatFlag(bool flag);
  [[maybe_unused]] void SetSortVariantsRowsFlag(bool flag);
  [[maybe_unused]] void SetSortVariantsColsFlag(bool flag);
  [[maybe_unused]] void SetVariantsCodecID(genie::core::AlgoID codec_id);
  [[maybe_unused]] void SetEncodePhasesDataFlag(bool flag);
  [[maybe_unused]] void SetTransposePhasesMatFlag(bool flag);
  [[maybe_unused]] void SetSortPhasesRowsFlag(bool flag);
  [[maybe_unused]] void SetSortPhasesColsFlag(bool flag);
  [[maybe_unused]] void SetPhasesCodecID(genie::core::AlgoID codec_id);

  static size_t GetSize() ;
  void Write(util::BitWriter& writer) const;
  void Write(core::Writer& writer) const;

  //TODO: Why we have read function here instead of a constructor?
  void read(util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::genotype

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------

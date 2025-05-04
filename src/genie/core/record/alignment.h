/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class Alignment final {
  std::string e_cigar_string_;          //!< @brief
  uint8_t reverse_comp_;                //!< @brief
  std::optional<uint8_t> flags_;        //!< @brief
  std::vector<int32_t> mapping_score_;  //!< @brief

 public:
  /**
   * @brief
   */
  Alignment(Alignment&&) = default;

  /**
   * @brief
   */
  Alignment& operator=(Alignment&&) = default;

  /**
   * @brief
   */
  Alignment(const Alignment&) = default;

  /**
   * @brief
   */
  Alignment& operator=(const Alignment&) = default;

  /**
   * @brief
   */
  ~Alignment() = default;

  /**
   * @brief
   * @param e_cigar_string
   * @param reverse_comp
   */
  Alignment(std::string&& e_cigar_string, uint8_t reverse_comp);

  /**
   * @brief
   * @param as_depth
   * @param extended_alignment
   * @param reader
   */
  Alignment(uint8_t as_depth, bool extended_alignment, util::BitReader& reader);

  /**
   * @brief
   */
  Alignment();

  /**
   * @brief
   * @param score
   */
  void AddMappingScore(int32_t score);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<int32_t>& GetMappingScores() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetECigar() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetRComp() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::optional<uint8_t> GetFlags() const;

  /**
   * @brief
   * @param flags
   */
  void SetFlags(std::optional<uint8_t> flags);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

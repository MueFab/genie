/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_LOCUS_H_
#define SRC_GENIE_CORE_LOCUS_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Refers to a specific position in the genome
 */
class Locus {
  std::string ref_name_;  //! @brief Reference sequence / chromosome name
  bool pos_present_;      //! @brief If false, no assumptions can be made about
                          //! start and end
  uint32_t start_;        //! @brief First base included in the locus
  uint32_t end_;          //! @brief First base not included in the locus

 public:
  /**
   * @brief Getter for reference name
   * @return refName
   */
  [[nodiscard]] const std::string& GetRef() const;

  /**
   * @brief Getter for starting position
   * @return start
   */
  [[nodiscard]] uint32_t GetStart() const;

  /**
   * @brief Getter for end position
   * @return end
   */
  [[nodiscard]] uint32_t GetEnd() const;

  /**
   * @brief If the locus contains any exact position
   * @return If true, start and end are valid.
   */
  [[nodiscard]] bool PositionPresent() const;

  /**
   * @brief Constructs a locus without a position [start - end] (sequence name
   * only)
   * @param ref Sequence name
   */
  explicit Locus(std::string ref);

  /**
   * @brief Constructs a locus with exact position
   * @param ref Sequence name
   * @param start Start position
   * @param end  End position
   */
  Locus(std::string ref, uint32_t start, uint32_t end);

  /**
   * @brief Parse a locus from a string in format "sequenceName:start-pos" or
   * "sequenceName"
   * @param string Input string
   * @return Constructed locus
   */
  static Locus FromString(const std::string& string);

  /**
   * @brief Convert into string in format "sequenceName:start-pos" or
   * "sequenceName"
   * @return Converted string
   */
  [[nodiscard]] std::string ToString() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_LOCUS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

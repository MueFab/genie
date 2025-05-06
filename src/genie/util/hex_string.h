/**
 * Copyright 2018-2024 The Genie Authors.
 * @file hex_string.h
 * @brief
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_HEX_STRING_H_
#define SRC_GENIE_UTIL_HEX_STRING_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Class representing a hexadecimal string.
 *
 * The `HexString` class provides functionality to handle and validate
 * hexadecimal strings. It includes methods for construction, assignment,
 * comparison, and output streaming.
 */
class HexString {
 public:
  /**
   * @brief Default constructor.
   *
   * Initializes an empty `HexString`.
   */
  HexString() = default;

  /**
   * @brief Constructor that initializes the `HexString` with a given string.
   *
   * @param input The input string to initialize the `HexString`.
   * @throws RuntimeException if the input string is not a valid hexadecimal
   * string.
   */
  explicit HexString(const std::string& input);

  /**
   * @brief Returns the string representation of the `HexString`.
   *
   * @return The string value of the `HexString`.
   */
  [[nodiscard]] const std::string& str() const;

  /**
   * @brief Sets the value of the `HexString`.
   *
   * @param input The input string to set.
   * @throws RuntimeException if the input string is not a valid hexadecimal
   * string.
   */
  void set(const std::string& input);

  /**
   * @brief Compares this `HexString` with another for equality.
   *
   * @param other The other `HexString` to compare with.
   * @return True if both `HexString` objects are equal, false otherwise.
   */
  [[nodiscard]]
  bool operator==(const HexString& other) const;

  /**
   * @brief Compares this `HexString` with another for inequality.
   *
   * @param other The other `HexString` to compare with.
   * @return True if both `HexString` objects are not equal, false otherwise.
   */
  [[nodiscard]]
  bool operator!=(const HexString& other) const;

  /**
   * @brief Outputs the `HexString` to an output stream.
   *
   * @param os The output stream to write to.
   * @param h The `HexString` to output.
   * @return The output stream after writing the `HexString`.
   */
  friend std::ostream& operator<<(std::ostream& os, const HexString& h);

 private:
  /// @brief The string value of the `HexString`.
  std::string value_;

  /// @brief Validates if a string is a valid hexadecimal string.
  static bool IsValidHex(const std::string& s);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  //  SRC_GENIE_UTIL_HEX_STRING_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

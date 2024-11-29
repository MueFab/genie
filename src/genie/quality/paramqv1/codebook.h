/**
 * Copyright 2018-2024 The Genie Authors.
 * @file codebook.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Codebook class for managing quality value
 * reconstruction entries.
 *
 * This file contains the declaration of the `Codebook` class, which is used for
 * storing and managing a list of quality value reconstruction entries. The
 * `Codebook` class provides methods for adding entries, serializing the
 * codebook to a bitstream, and comparing codebooks within the GENIE framework.
 *
 * @details The `Codebook` class is part of the GENIE quality value coding
 * module and is designed to store quality value entries that map coded quality
 * values to their reconstructed values. This mapping is used during the
 * encoding and decoding of quality values in genomic records.
 */

#ifndef SRC_GENIE_QUALITY_PARAMQV1_CODEBOOK_H_
#define SRC_GENIE_QUALITY_PARAMQV1_CODEBOOK_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------
namespace genie::quality::paramqv1 {

/**
 * @brief Represents a codebook for quality value reconstruction.
 *
 * The `Codebook` class is responsible for storing a list of quality value
 * entries that map coded quality values to their corresponding reconstructed
 * values. The class supports serialization, deserialization, and comparison of
 * codebooks, and allows for dynamic addition of new entries.
 */
class Codebook final {
  std::vector<uint8_t>
      qv_recon_;  //!< @brief The list of quality value reconstruction entries.

 public:
  /**
   * @brief Compares two codebooks for equality.
   *
   * This operator compares the current codebook with another to determine if
   * they contain the same quality value entries.
   *
   * @param ps The other codebook to compare with.
   * @return True if the codebooks are equal, false otherwise.
   */
  bool operator==(const Codebook& ps) const;

  /**
   * @brief Constructs a Codebook object from a bit reader.
   *
   * This constructor initializes a `Codebook` object by reading the quality
   * value entries from the provided `BitReader` stream. It is used to
   * deserialize a codebook from a bitstream.
   *
   * @param reader The `BitReader` to read the codebook entries from.
   */
  explicit Codebook(util::BitReader& reader);

  /**
   * @brief Constructs a Codebook object with two initial quality value entries.
   *
   * This constructor initializes a `Codebook` object with two specific quality
   * value entries, allowing for a basic codebook setup.
   *
   * @param v1 The first quality value entry to be added.
   * @param v2 The second quality value entry to be added.
   */
  Codebook(uint8_t v1, uint8_t v2);

  /**
   * @brief Adds a new quality value entry to the codebook.
   *
   * This method allows users to add a new quality value entry to the codebook,
   * expanding the list of quality values that can be mapped.
   *
   * @param entry The quality value entry to be added.
   */
  void AddEntry(uint8_t entry);

  /**
   * @brief Retrieves the list of quality value entries.
   *
   * This method returns a constant reference to the vector of quality value
   * entries stored in the codebook. It allows users to access the entries
   * without modifying them.
   *
   * @return A constant reference to the vector of quality value entries.
   */
  [[nodiscard]] const std::vector<uint8_t>& GetEntries() const;

  /**
   * @brief Writes the codebook entries to a bitstream.
   *
   * This method serializes the internal state of the `Codebook` object and
   * writes it to the provided `BitWriter` stream. It is used to store the
   * codebook for later decoding or for transmission.
   *
   * @param writer The `BitWriter` to write the codebook data to.
   */
  void write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------
}  // namespace genie::quality::paramqv1

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_PARAMQV1_CODEBOOK_H_

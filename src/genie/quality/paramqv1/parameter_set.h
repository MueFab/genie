/**
 * Copyright 2018-2024 The Genie Authors.
 * @file parameter_set.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ParameterSet class for managing quality value
 * codebook configurations.
 *
 * This file contains the declaration of the `ParameterSet` class, which is used
 * for managing and storing a set of quality value codebooks within the GENIE
 * framework. The `ParameterSet` class provides methods for serialization,
 * deserialization, and comparison of codebook configurations.
 *
 * @details The `ParameterSet` class is designed to handle collections of
 * quality value codebooks, allowing users to add new codebooks, serialize them
 * to a bitstream, and retrieve the stored codebooks for quality value coding in
 * genomic records.
 */

#ifndef SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_
#define SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/quality/paramqv1/codebook.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

/**
 * @brief Manages a set of quality value codebooks for the GENIE framework.
 *
 * The `ParameterSet` class is responsible for managing and storing a collection
 * of quality value codebooks. It provides methods for adding new codebooks,
 * serializing the entire set to a bitstream, and retrieving the stored
 * codebooks for use in quality value coding. The class supports equality
 * comparison to facilitate comparison of different parameter sets.
 */
class ParameterSet final {
  /// The collection of quality value codebooks.
  std::vector<Codebook> qv_codebooks_;

 public:
  /**
   * @brief Compares two parameter sets for equality.
   *
   * This operator compares the current parameter set with another to determine
   * if they are equal, based on the stored codebooks.
   *
   * @param ps The other parameter set to compare with.
   * @return True if the parameter sets are equal, false otherwise.
   */
  bool operator==(const ParameterSet& ps) const;

  /**
   * @brief Constructs a ParameterSet object from a bit reader.
   *
   * This constructor initializes a `ParameterSet` object by reading the
   * configuration data from the provided `BitReader` stream. It is used to
   * deserialize the parameter set from a bitstream.
   *
   * @param reader The `BitReader` to read the configuration data from.
   */
  explicit ParameterSet(util::BitReader& reader);

  /**
   * @brief Default constructor for creating an empty parameter set.
   *
   * This constructor initializes an empty `ParameterSet` object with no
   * codebooks.
   */
  ParameterSet() = default;

  /**
   * @brief Retrieves the collection of quality value codebooks.
   *
   * This method returns a constant reference to the vector of codebooks stored
   * in the parameter set. It allows users to access the collection without
   * modifying it.
   *
   * @return A constant reference to the vector of `Codebook` objects.
   */
  [[nodiscard]] const std::vector<Codebook>& GetCodebooks() const;

  /**
   * @brief Adds a new codebook to the parameter set.
   *
   * This method allows users to add a new codebook to the parameter set. The
   * codebook is moved into the internal storage, avoiding unnecessary copies.
   *
   * @param book The codebook to be added to the parameter set.
   */
  void AddCodeBook(Codebook&& book);

  /**
   * @brief Writes the parameter set to a bitstream.
   *
   * This method serializes the internal state of the `ParameterSet` object and
   * writes it to the provided `BitWriter` stream. It is used to store the
   * parameter set for later decoding or for transmission.
   *
   * @param writer The `BitWriter` to write the parameter set data to.
   */
  void write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::paramqv1

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_

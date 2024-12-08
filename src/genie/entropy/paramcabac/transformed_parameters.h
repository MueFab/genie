/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the `TransformedParameters` class for managing CABAC transform
 * parameters in entropy coding.
 * @details This class is used to represent and manage the various transform
 * parameters used in CABAC-based entropy coding schemes within the GENIE
 * framework. It provides serialization, deserialization, and parameter
 *          management for different transform types, ensuring that the required
 * configurations for entropy coding are correctly applied and stored.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_

// -----------------------------------------------------------------------------

#include <optional>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "nlohmann/json.hpp"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Encapsulates parameters for various CABAC transformations.
 * @details The `TransformedParameters` class manages different transform
 * parameters for CABAC encoding, including settings for equality coding, match
 * coding, run-length encoding, and merge coding. The class provides methods for
 * serialization, deserialization, JSON conversion, and equality checks.
 */
class TransformedParameters final {
 public:
  /**
   * @brief Enumeration of supported transform types.
   */
  enum class TransformIdSubseq : uint8_t {
    NO_TRANSFORM = 0,     //!< @brief No transform is applied.
    EQUALITY_CODING = 1,  //!< @brief Equality coding transform.
    MATCH_CODING = 2,     //!< @brief Match coding transform.
    RLE_CODING = 3,       //!< @brief Run-Length Encoding (RLE) transform.
    MERGE_CODING = 4      //!< @brief Merge coding transform.
  };

  /**
   * @brief Serializes the transform parameters to a `BitWriter`.
   * @param writer The `BitWriter` object used for serializing the data.
   */
  void write(util::BitWriter& writer) const;

  /**
   * @brief Default constructor.
   */
  TransformedParameters();

  /**
   * @brief Deserialization constructor using a `BitReader`.
   * @param reader The `BitReader` object used for deserializing the data.
   */
  explicit TransformedParameters(util::BitReader& reader);

  /**
   * @brief Constructs the transform parameters from given parameters.
   * @param transform_id_subseq The type of transform to apply.
   * @param param The primary parameter value for the transform.
   */
  TransformedParameters(const TransformIdSubseq& transform_id_subseq,
                        uint16_t param);

  /**
   * @brief Retrieves the number of streams affected by the current transform.
   * @return The number of streams.
   */
  [[nodiscard]] size_t GetNumStreams() const;

  /**
   * @brief Retrieves the current transform type.
   * @return The transform type identifier.
   */
  [[nodiscard]] TransformIdSubseq GetTransformIdSubseq() const;

  /**
   * @brief Retrieves the primary parameter for the current transform.
   * @return The primary transform parameter.
   */
  [[nodiscard]] uint16_t GetParam() const;

  /**
   * @brief Retrieves the merge coding shift sizes if applicable.
   * @return A vector of shift sizes for merge coding.
   */
  [[nodiscard]] std::vector<uint8_t> GetMergeCodingShiftSizes() const;

  /**
   * @brief Sets the merge coding shift sizes.
   * @param merge_codingshift_sizes A vector containing the shift sizes for
   * merge coding.
   */
  void SetMergeCodingShiftSizes(std::vector<uint8_t> merge_codingshift_sizes);

  /**
   * @brief Equality operator for comparing two `TransformedParameters` objects.
   * @param val The other `TransformedParameters` object to compare with.
   * @return True if both objects are equal, otherwise false.
   */
  bool operator==(const TransformedParameters& val) const;

  /**
   * @brief Converts the current transform parameters to a JSON object.
   * @return A JSON representation of the transform parameters.
   */
  [[nodiscard]] nlohmann::json ToJson() const;

  /**
   * @brief Constructs the transform parameters from a JSON object.
   * @param json The JSON object containing the transform parameters.
   */
  explicit TransformedParameters(const nlohmann::json& json);

 private:
  TransformIdSubseq
      transform_id_subseq_;  //!< @brief Type of transform applied to the data.
  std::optional<uint16_t>
      match_coding_buffer_size_;  //!< @brief Optional buffer
                                  //!< Size for match coding.
  std::optional<uint8_t>
      rle_coding_guard_;  //!< @brief Optional guard value for RLE coding.
  std::optional<uint8_t>
      merge_coding_subseq_count_;  //!< @brief Optional subsequence count for
                                   //!< merge coding.
  std::vector<uint8_t>
      merge_coding_shift_size_;  //!< @brief Optional shift sizes for merge
                                 //!< coding subsequences.
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

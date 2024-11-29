/**
 * Copyright 2018-2024 The Genie Authors.
 * @file qv_coding_config_1.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the QualityValues1 class for handling quality value
 * coding configurations.
 *
 * This file contains the declaration of the `QualityValues1` class, which is a
 * specialized implementation of the `core::parameter::QualityValues` class
 * within the GENIE framework. The `QualityValues1` class provides methods for
 * configuring and managing quality values using different parameter sets and
 * preset configurations.
 *
 * @details The `QualityValues1` class supports encoding and decoding of quality
 * values using various predefined presets and codebooks. It allows users to
 * specify quality value parameter sets or use standard presets for quality
 * value coding in genomic records.
 */

#ifndef SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_
#define SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <optional>

#include "genie/core/parameter/parameter_set.h"
#include "genie/core/parameter/quality_values.h"
#include "genie/quality/paramqv1/parameter_set.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

constexpr uint8_t kModeQv1 = 1;

/**
 * @brief Implements quality value configurations with parameterized coding
 * strategies.
 *
 * The `QualityValues1` class is responsible for handling quality value
 * configurations in the GENIE framework. It provides methods for encoding,
 * decoding, and managing quality value parameter sets using both custom
 * configurations and predefined presets. The class supports cloning,
 * comparison, and serialization of quality value configurations.
 */
class QualityValues1 final : public core::parameter::QualityValues {
 public:
  /**
   * @brief Enumeration of quality value preset IDs.
   *
   * The `QualityParametersPresetId` enum defines different preset
   * configurations for quality value coding. These presets specify the range
   * and offset of quality values based on the format used.
   */
  enum class QualityParametersPresetId {
    ASCII = 0,  //!< @brief ASCII quality values.
    OFFSET33_RANGE41 =
        1,  //!< @brief Quality values with offset 33 and range 41.
    OFFSET64_RANGE40 =
        2  //!< @brief Quality values with offset 64 and range 40.
  };

  /**
   * @brief Constructs a `QualityValues1` object from a descriptor and a bit
   * reader.
   *
   * This constructor initializes a `QualityValues1` object using the given
   * descriptor and `BitReader`. It reads the necessary data from the input
   * stream to set up the internal state of the object.
   *
   * @param desc The descriptor of the coding configuration.
   * @param reader The bit reader to read the configuration data from.
   */
  explicit QualityValues1(core::GenDesc desc, util::BitReader& reader);

  /**
   * @brief Constructs a `QualityValues1` object with a preset ID and a reverse
   * flag.
   *
   * This constructor initializes a `QualityValues1` object using a predefined
   * preset ID and a reverse flag, which indicates whether the coding should be
   * applied in reverse order.
   *
   * @param quality_param_preset_id The preset ID for quality value coding.
   * @param reverse_flag Flag indicating if the coding should be applied in
   * reverse.
   */
  explicit QualityValues1(QualityParametersPresetId quality_param_preset_id,
                          bool reverse_flag);

  /**
   * @brief Sets the parameter set for quality value coding.
   *
   * This method allows users to specify a custom parameter set for quality
   * value coding, overriding the default or preset configurations.
   *
   * @param parameter_set_qualities The custom parameter set to be used for
   * quality value coding.
   */
  void SetQualityParameters(ParameterSet&& parameter_set_qualities);

  /**
   * @brief Writes the quality value configuration to a bit stream.
   *
   * This method serializes the internal state of the `QualityValues1` object
   * and writes it to the provided `BitWriter`. It is used to store the
   * configuration for later decoding.
   *
   * @param writer The `BitWriter` to write the configuration data to.
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief Creates a copy of the current `QualityValues1` object.
   *
   * This method returns a unique pointer to a newly created `QualityValues1`
   * object that is a deep copy of the current object.
   *
   * @return A unique pointer to the cloned `QualityValues1` object.
   */
  [[nodiscard]] std::unique_ptr<QualityValues> Clone() const override;

  /**
   * @brief Creates a `QualityValues1` object from a descriptor and a bit
   * reader.
   *
   * This static method initializes a `QualityValues1` object using the provided
   * descriptor and bit reader, setting up the internal configuration based on
   * the input data.
   *
   * @param desc The descriptor of the coding configuration.
   * @param reader The bit reader to read the configuration data from.
   * @return A unique pointer to the created `QualityValues1` object.
   */
  static std::unique_ptr<QualityValues> create(core::GenDesc desc,
                                               util::BitReader& reader);

  /**
   * @brief Retrieves a predefined codebook based on the given preset ID.
   *
   * This static method returns a reference to a `Codebook` object that
   * corresponds to the specified preset ID. The codebook defines the mapping
   * and encoding strategies for quality values using the given preset.
   *
   * @param id The preset ID for which to retrieve the codebook.
   * @return A reference to the corresponding `Codebook` object.
   */
  static const Codebook& GetPresetCodebook(QualityParametersPresetId id);

  /**
   * @brief Retrieves the default quality value set for a given class type.
   *
   * This static method returns a `QualityValues1` object configured with
   * default settings for the specified class type (e.g., paired-end,
   * single-end).
   *
   * @param type The class type for which to retrieve the default quality value
   * set.
   * @return A unique pointer to the configured `QualityValues1` object.
   */
  static std::unique_ptr<QualityValues> GetDefaultSet(
      core::record::ClassType type);

  /**
   * @brief Gets the number of codebooks used in the current configuration.
   *
   * This method returns the number of codebooks defined in the internal
   * parameter set for quality value coding.
   *
   * @return The number of codebooks in the configuration.
   */
  [[nodiscard]] size_t GetNumberCodeBooks() const;

  /**
   * @brief Retrieves the codebook at the specified index.
   *
   * This method returns a reference to the codebook at the specified index in
   * the internal parameter set.
   *
   * @param id The index of the codebook to retrieve.
   * @return A reference to the codebook at the specified index.
   */
  [[nodiscard]] const Codebook& GetCodebook(size_t id) const;

  /**
   * @brief Gets the number of subsequences in the quality value configuration.
   *
   * This method returns the number of subsequences defined for quality value
   * coding, indicating how many distinct subsequences are handled by the
   * current configuration.
   *
   * @return The number of subsequences.
   */
  [[nodiscard]] size_t GetNumSubsequences() const override;

  /**
   * @brief Compares the current quality value configuration with another.
   *
   * This method checks if the given `QualityValues1` object is equal to the
   * current one, comparing their internal parameters and configurations.
   *
   * @param qv The other `QualityValues1` object to compare with.
   * @return True if the objects are equal, false otherwise.
   */
  bool Equals(const QualityValues* qv) const override;

 private:
  std::optional<ParameterSet>
      parameter_set_quality_;  //!< @brief Optional custom parameter set for
                               //!< quality values.
  std::optional<QualityParametersPresetId>
      quality_parameter_preset_id_;  //!< @brief Optional preset ID for
                                     //!< predefined configurations.
};

// -----------------------------------------------------------------------------
}  // namespace genie::quality::paramqv1

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_

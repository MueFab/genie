/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the GabacSeqConfSet class for managing the gabac
 * sequence configurations.
 *
 * This file contains the declaration of the GabacSeqConfSet class, which
 * provides an interface for managing configuration sets used in gabac encoding
 * and decoding. It includes functionality to translate between gabac
 * configurations and MPEG-G parameter sets, and offers methods to load, store,
 * and retrieve configuration data.
 *
 * @details The GabacSeqConfSet class maintains a set of configurations for
 * genomic descriptors, handling their translation to and from MPEG-G P2
 * parameter sets. It provides methods to extract configurations, modify them,
 * and interact with other components in the gabac library. The class ensures
 * compatibility by validating the configurations against predefined modes and
 * states.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_
#define SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_

// -----------------------------------------------------------------------------

#include <utility>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/entropy/paramcabac/decoder.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/entropy/paramcabac/transformed_sub_seq.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Manages a set of gabac configurations for genomic descriptors.
 *
 * The GabacSeqConfSet class handles storing, loading, and manipulating
 * configurations used for encoding and decoding genomic data with gabac. It can
 * translate configurations to and from MPEG-G parameter sets, ensuring
 * compatibility and providing an easy-to-use interface.
 */
class GabacSeqConfSet {
 private:
  typedef std::vector<gabac::EncodingConfiguration>
      SeqConf;                 //!< Configuration for one genomic descriptor.
  std::vector<SeqConf> conf_;  //!< Configuration for all genomic descriptors.

  // Type aliases for readability.
  using DescriptorSubsequenceCfg = entropy::paramcabac::Subsequence;
  using ParameterSet = core::parameter::ParameterSet;

  /**
   * @brief Loads a specific decoder configuration from the parameter set.
   *
   * This function extracts a paramcabac::DecoderConfigurationCabac from the
   * provided ParameterSet. It ensures that the configuration is not
   * class-specific and is of the correct mode.
   *
   * @tparam T The type of decoder configuration to extract.
   * @param parameter_set The input parameter set from which to extract the
   * configuration.
   * @param descriptor_id The descriptor identifier to use for locating the
   * configuration.
   * @return A reference to the extracted DecoderConfigurationCabac.
   */
  template <typename T>
  static const T& LoadDescriptorDecoderCfg(const ParameterSet& parameter_set,
                                           core::GenDesc descriptor_id);

 public:
  /**
   * @brief Retrieves the gabac configuration for the specified subsequence.
   * @param sub The subsequence identifier to retrieve the configuration for.
   * @return The gabac configuration corresponding to the specified
   * subsequence.
   */
  [[nodiscard]] const gabac::EncodingConfiguration& GetConfAsGabac(
      core::GenSubIndex sub) const;

  /**
   * @brief Retrieves a modifiable reference to the gabac configuration for
   * the specified subsequence.
   * @param sub The subsequence identifier to retrieve the configuration for.
   * @return A reference to the gabac configuration corresponding to the
   * specified subsequence.
   */
  gabac::EncodingConfiguration& GetConfAsGabac(core::GenSubIndex sub);

  /**
   * @brief Sets a gabac configuration for the specified subsequence.
   * @param sub The subsequence identifier to set the configuration for.
   * @param subseq_cfg The descriptor subsequence configuration to set.
   */
  void SetConfAsGabac(core::GenSubIndex sub,
                      DescriptorSubsequenceCfg&& subseq_cfg);

  /**
   * @brief Constructs a GabacSeqConfSet with a default configuration.
   *
   * Creates a default gabac configuration set that is guaranteed to work with
   * basic settings (bypass mode, no transformation, binary binarization).
   */
  GabacSeqConfSet();

  /**
   * @brief Stores the complete set of gabac configurations into an MPEG-G
   * parameter set.
   * @param parameter_set The parameter set object to store the configurations
   * into.
   */
  [[maybe_unused]] void StoreParameters(ParameterSet& parameter_set) const;

  /**
   * @brief Stores gabac configurations for a specific descriptor in a
   * parameter set.
   * @param desc The genomic descriptor to store configurations for.
   * @param parameter_set The parameter set to store the configurations into.
   */
  void StoreParameters(
      core::GenDesc desc,
      core::parameter::DescriptorSubSequenceCfg& parameter_set) const;

  /**
   * @brief Loads a complete set of gabac configurations from an MPEG-G
   * parameter set.
   * @param parameter_set The parameter set object to load the configurations
   * from.
   */
  [[maybe_unused]] void LoadParameters(const ParameterSet& parameter_set);

  /**
   * @brief Fills a decoder configuration with the settings from the internal
   * configuration set.
   * @tparam T The type of the decoder configuration to fill.
   * @param desc The descriptor properties for the genomic data.
   * @param decoder_config The decoder configuration object to populate.
   */
  template <typename T>
  void FillDecoder(const core::GenomicDescriptorProperties& desc,
                   T& decoder_config) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/gabac_seq_conf_set.impl.h"

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUB_SEQUENCE_CFG_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUB_SEQUENCE_CFG_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/parameter/descriptor.h"
#include "genie/util/bit_reader.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class DescriptorSubSequenceCfg final {
 public:
  /**
   * @brief
   */
  DescriptorSubSequenceCfg();

  /**
   * @brief
   * @param cfg
   */
  DescriptorSubSequenceCfg(const DescriptorSubSequenceCfg& cfg);

  /**
   * @brief
   * @param cfg
   */
  DescriptorSubSequenceCfg(DescriptorSubSequenceCfg&& cfg) noexcept;

  /**
   * @brief
   * @param cfg
   * @return
   */
  DescriptorSubSequenceCfg& operator=(const DescriptorSubSequenceCfg& cfg);

  /**
   * @brief
   * @param cfg
   * @return
   */
  DescriptorSubSequenceCfg& operator=(DescriptorSubSequenceCfg&& cfg) noexcept;

  /**
   * @brief
   * @param num_classes
   * @param desc
   * @param reader
   */
  DescriptorSubSequenceCfg(size_t num_classes, GenDesc desc,
                           util::BitReader& reader);

  /**
   * @brief
   */
  ~DescriptorSubSequenceCfg() = default;

  /**
   * @brief
   * @param index
   * @param conf
   */
  void SetClassSpecific(uint8_t index, std::unique_ptr<Descriptor> conf);

  /**
   * @brief
   * @param conf
   */
  void Set(std::unique_ptr<Descriptor> conf);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const Descriptor& Get() const;

  /**
   * @brief
   * @param index
   * @return
   */
  [[nodiscard]] const Descriptor& GetClassSpecific(uint8_t index) const;

  /**
   * @brief
   * @param num_classes
   */
  void EnableClassSpecific(uint8_t num_classes);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsClassSpecific() const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param cfg
   * @return
   */
  bool operator==(const DescriptorSubSequenceCfg& cfg) const;

 private:
  /**
   * @brief
   * @param cfg
   * @return
   */
  [[nodiscard]] bool DescComp(const DescriptorSubSequenceCfg& cfg) const;

  bool class_specific_dec_cfg_flag_;  //!< @brief
  std::vector<std::unique_ptr<Descriptor>>
      descriptor_configurations_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_SUB_SEQUENCE_CFG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
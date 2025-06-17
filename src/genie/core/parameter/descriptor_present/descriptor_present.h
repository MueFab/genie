/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// -----------------------------------------------------------------------------

#include <memory>

#include "genie/core/parameter/descriptor.h"
#include "genie/core/parameter/descriptor_present/decoder.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class DescriptorPresent final : public Descriptor {
  std::unique_ptr<Decoder> decoder_configuration_;  //!< @brief

 public:
  static constexpr int8_t kPresent = 0;  //!< @brief

  /**
   * @brief
   * @param desc
   * @return
   */
  bool Equals(const Descriptor* desc) const override;

  /**
   * @brief
   */
  DescriptorPresent();

  /**
   * @brief
   * @param desc
   * @param reader
   */
  explicit DescriptorPresent(GenDesc desc, util::BitReader& reader);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<Descriptor> Clone() const override;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief
   * @param conf
   */
  void SetDecoder(std::unique_ptr<Decoder> conf);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const Decoder& GetDecoder() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DESCRIPTOR_PRESENT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
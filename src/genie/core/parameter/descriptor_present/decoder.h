/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_

// -----------------------------------------------------------------------------

#include <memory>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

/**
 * @brief
 */
class Decoder {
 public:
  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<Decoder> Clone() const = 0;

  /**
   * @brief
   * @param encoding_mode_id
   */
  explicit Decoder(uint8_t encoding_mode_id);

  /**
   * @brief
   */
  virtual ~Decoder() = default;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetMode() const;

  /**
   * @brief
   * @param dec
   * @return
   */
  virtual bool Equals(const Decoder* dec) const;

 protected:
  uint8_t encoding_mode_id_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_PRESENT_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

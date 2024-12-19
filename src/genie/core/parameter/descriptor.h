/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_
#define SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// -----------------------------------------------------------------------------

#include <memory>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

/**
 * @brief
 */
class Descriptor {
 public:
  /**
   * @brief
   * @param dec_cfg_preset
   */
  explicit Descriptor(uint8_t dec_cfg_preset);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<Descriptor> Clone() const = 0;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param desc
   * @param reader
   * @return
   */
  static std::unique_ptr<Descriptor> Factory(GenDesc desc,
                                             util::BitReader& reader);

  /**
   * @brief
   */
  virtual ~Descriptor() = default;

  /**
   * @brief
   * @param desc
   * @return
   */
  virtual bool Equals(const Descriptor* desc) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetPreset() const;

 protected:
  uint8_t dec_cfg_preset_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DESCRIPTOR_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

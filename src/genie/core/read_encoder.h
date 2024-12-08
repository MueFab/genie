/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_READ_ENCODER_H_
#define SRC_GENIE_CORE_READ_ENCODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/core/module.h"
#include "genie/core/name_encoder.h"
#include "genie/core/qv_encoder.h"
#include "genie/util/side_selector.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief The basic interface for modules encoding the plain read data
 */
class ReadEncoder : public Module<record::Chunk, AccessUnit> {
 public:
  using qv_selector = util::SideSelector<QvEncoder, QvEncoder::qv_coded,
                                         const record::Chunk&>;  //!< @brief
  using name_selector =
      util::SideSelector<NameEncoder,
                         std::tuple<AccessUnit::Descriptor, stats::PerfStats>,
                         const record::Chunk&>;  //!< @brief
  using entropy_selector =
      util::SideSelector<EntropyEncoder, EntropyEncoder::entropy_coded,
                         AccessUnit::Descriptor&>;  //!< @brief

 protected:
  qv_selector* qvcoder_{};            //!< @brief
  name_selector* namecoder_{};        //!< @brief
  entropy_selector* entropycoder_{};  //!< @brief
  bool write_out_streams_;

 public:
  /**
   * @brief
   * @param coder
   */
  virtual void SetQvCoder(qv_selector* coder);

  /**
   * @brief
   * @param coder
   */
  virtual void SetNameCoder(name_selector* coder);

  /**
   * @brief
   * @param coder
   */
  virtual void SetEntropyCoder(entropy_selector* coder);

  /**
   * @brief
   * @param entropycoder
   * @param a
   * @param write_raw
   * @return
   */
  static AccessUnit EntropyCodeAu(entropy_selector* entropycoder,
                                  AccessUnit&& a, bool write_raw);

  /**
   * @brief
   * @param a
   * @return
   */
  AccessUnit EntropyCodeAu(AccessUnit&& a) const;

  /**
   * @brief For polymorphic destruction
   */
  ~ReadEncoder() override = default;

  /**
   * @brief
   * @param write_out_streams
   */
  explicit ReadEncoder(const bool write_out_streams)
      : write_out_streams_(write_out_streams) {}
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_READ_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

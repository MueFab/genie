/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_READ_DECODER_H_
#define SRC_GENIE_CORE_READ_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_decoder.h"
#include "genie/core/module.h"
#include "genie/core/name_decoder.h"
#include "genie/core/qv_decoder.h"
#include "genie/core/record/chunk.h"
#include "genie/util/side_selector.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief The basic interface for modules decoding the plain read data
 */
class ReadDecoder : public Module<AccessUnit, record::Chunk> {
 public:
  using qv_selector = util::SideSelector<
      QvDecoder, std::tuple<std::vector<std::string>, stats::PerfStats>,
      const parameter::QualityValues&, const std::vector<std::string>&,
      const std::vector<uint64_t>&, AccessUnit::Descriptor&>;  //!< @brief
  using name_selector =
      util::SideSelector<NameDecoder,
                         std::tuple<std::vector<std::string>, stats::PerfStats>,
                         AccessUnit::Descriptor&>;  //!< @brief
  using entropy_selector = util::SideSelector<
      EntropyDecoder, std::tuple<AccessUnit::Descriptor, stats::PerfStats>,
      const parameter::DescriptorSubSequenceCfg&, AccessUnit::Descriptor&,
      bool>;  //!< @brief

 protected:
  qv_selector* qvcoder_{};            //!< @brief
  name_selector* namecoder_{};        //!< @brief
  entropy_selector* entropycoder_{};  //!< @brief

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
   * @param select
   * @param a
   * @param mm_coder_enabled
   * @return
   */
  static AccessUnit EntropyCodeAu(entropy_selector* select, AccessUnit&& a,
                                  bool mm_coder_enabled);

  /**
   * @brief
   * @param a
   * @param mm_coder_enabled
   * @return
   */
  AccessUnit EntropyCodeAu(AccessUnit&& a, bool mm_coder_enabled) const;

  /**
   * @brief For polymorphic destruction
   */
  ~ReadDecoder() override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_READ_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

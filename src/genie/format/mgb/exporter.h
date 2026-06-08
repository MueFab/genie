/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_EXPORTER_H_
#define SRC_GENIE_FORMAT_MGB_EXPORTER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/format_exporter.h"
#include "genie/core/format_exporter_compressed.h"
#include "genie/core/stats/perf_stats.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/util/drain.h"
#include "genie/util/ordered_lock.h"
#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class Exporter : public core::FormatExporterCompressed {
 private:
  util::BitWriter writer;                                      //!< @brief
  util::OrderedLock lock;                                      //!< @brief
  size_t id_ctr;                                               //!< @brief
  std::vector<core::parameter::ParameterSet> parameter_stash;  //!< @brief

 public:
  /**
   * @brief
   * @param file
   */
  explicit Exporter(std::ostream* file);

  /**
   * @brief
   * @param t
   * @param id
   */
  void FlowIn(core::AccessUnit&& t, const genie::util::Section& id) override;

  /**
   * @brief
   * @param id
   */
  void SkipIn(const genie::util::Section& id) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_EXPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

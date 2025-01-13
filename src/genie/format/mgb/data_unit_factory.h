/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_
#define SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_

// -----------------------------------------------------------------------------

#include <map>

#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/raw_reference.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

// -----------------------------------------------------------------------------

class Importer;

/**
 * @brief
 */
class DataUnitFactory {
  std::map<size_t, core::parameter::EncodingSet> parameters_;  //!< @brief
  core::ReferenceManager* refmgr_;                             //!< @brief
  Importer* importer_;                                         //!< @brief
  bool reference_only_;                                         //!< @brief

 public:
  /**
   * @brief
   * @param mgr
   * @param importer
   * @param ref
   */
  explicit DataUnitFactory(core::ReferenceManager* mgr,
                           Importer* importer, bool ref);

  /**
   * @brief
   * @param id
   * @return
   */
  [[nodiscard]] const core::parameter::EncodingSet& GetParams(size_t id) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::map<size_t, core::parameter::EncodingSet>&
  GetParams() const;

  /**
   * @brief
   * @param bit_reader
   * @return
   */
  std::optional<AccessUnit> read(util::BitReader& bit_reader);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

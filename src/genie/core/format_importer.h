/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_IMPORTER_H_
#define SRC_GENIE_CORE_FORMAT_IMPORTER_H_

// -----------------------------------------------------------------------------

#include <mutex>  //NOLINT

#include "genie/core/access_unit.h"
#include "genie/core/classifier.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Interface for importers of various file formats. Note that each
 * importer has to convert to mpegg-records
 */
class FormatImporter : public util::OriginalSource,
                       public util::Source<record::Chunk> {
  Classifier* classifier_ = nullptr;  //!< @brief
  bool flushing_{false};              //!< @brief

 protected:
  /**
   * @brief
   * @param classifier
   * @return
   */
  virtual bool PumpRetrieve(Classifier* classifier) = 0;

 public:
  /**
   * @brief
   * @param classifier
   */
  void SetClassifier(Classifier* classifier);

  /**
   * @brief
   * @param id
   * @param lock
   * @return
   */
  bool Pump(uint64_t& id, std::mutex& lock) override;

  /**
   * @brief
   */
  void FlushIn(uint64_t& pos) override;

  /**
   * @brief
   */
  ~FormatImporter() override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_IMPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

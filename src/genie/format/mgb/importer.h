/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_IMPORTER_H_
#define SRC_GENIE_FORMAT_MGB_IMPORTER_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/format_importer_compressed.h"
#include "genie/core/ref_decoder.h"
#include "genie/core/reference_source.h"
#include "genie/format/mgb/data_unit_factory.h"
#include "genie/util/bit_reader.h"
#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class Importer final : public core::FormatImporterCompressed,
                       public core::ReferenceSource {
  ///
  util::BitReader reader_;

  ///
  std::mutex lock_;

  ///
  DataUnitFactory factory_;

  ///
  core::ReferenceManager* ref_manager_;

  ///
  core::RefDecoder* decoder_;

  ///
  uint64_t file_size_;

  ///
  float last_progress_;

  /**
   * @brief
   * @param au
   * @return
   */
  core::AccessUnit ConvertAu(AccessUnit&& au) const;

 public:
  /**
   * @brief
   * @param file
   * @param manager
   * @param ref_decoder
   * @param ref_only
   */
  explicit Importer(std::istream& file, core::ReferenceManager* manager,
                    core::RefDecoder* ref_decoder, bool ref_only);

  /**
   * @brief
   * @param id
   * @param lock
   * @return
   */
  bool Pump(uint64_t& id, std::mutex& lock) override;

  /**
   * @brief
   * @param raw
   * @param f_pos
   * @param start
   * @param end
   * @return
   */
  std::string GetRef(bool raw, size_t f_pos, size_t start, size_t end);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_IMPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

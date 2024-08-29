/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_
#define SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FormatImporterCompressed : public util::Source<AccessUnit>, public util::OriginalSource {
 public:
    /**
     * @brief
     */
    ~FormatImporterCompressed() override = default;

    /**
     * @brief
     */
    void flushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_
#define SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

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
    void FlushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_IMPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

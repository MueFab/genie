/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_COMPRESSED_H
#define GENIE_FORMAT_IMPORTER_COMPRESSED_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit-payload.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FormatImporterCompressed : public util::Source<AccessUnitPayload>, public util::OriginalSource {
   public:
    /**
     *
     */
    ~FormatImporterCompressed() override = default;

    /**
     *
     */
    void flushIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_COMPRESSED_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
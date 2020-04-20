/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_H
#define GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include <genie/core/access-unit-payload.h>
#include "record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for importers of various file formats. Note that each importer has to convert to mpegg-records
 */
class FormatImporter : public util::Source<record::Chunk>, public util::OriginalSource {};

class FormatImporterCompressed : public util::Source<core::AccessUnitPayload>, public util::OriginalSource {};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
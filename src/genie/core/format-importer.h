/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_H
#define GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

class FormatImporter : public util::Source<record::MpeggChunk>, public util::OriginalSource {};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
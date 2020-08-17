/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_IMPORTER_NULL_H
#define GENIE_FORMAT_IMPORTER_NULL_H

// ---------------------------------------------------------------------------------------------------------------------

#include "format-importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
/**
 *
 */
class NullImporter : public FormatImporter {
   protected:
    /**
     *
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(Classifier* _classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_IMPORTER_NULL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
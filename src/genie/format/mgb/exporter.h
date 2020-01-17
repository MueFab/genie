/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG2_EXPORTER_H
#define GENIE_MPEGG2_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/drain.h>

#include <genie/core/access-unit-raw.h>
#include <memory>
#include <vector>

#include <genie/core/access-unit-payload.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/ordered-section.h>
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Exporter : public util::Drain<core::AccessUnitPayload> {
   private:
    util::BitWriter writer;
    util::OrderedLock lock;

   public:
    explicit Exporter(std::ostream* _file);
    void flowIn(core::AccessUnitPayload&& t, size_t id) override;
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
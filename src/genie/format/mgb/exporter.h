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
#include <genie/core/format-exporter-compressed.h>
#include <genie/core/format-exporter.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/ordered-section.h>
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Exporter : public core::FormatExporterCompressed {
   private:
    util::BitWriter writer;
    util::OrderedLock lock;

   public:
    explicit Exporter(std::ostream* _file);
    void flowIn(core::AccessUnitPayload&& t, const genie::util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

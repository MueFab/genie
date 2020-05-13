/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_IMPORTER_H
#define GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/ordered-section.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include <map>
#include "data-unit-factory.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Importer : public util::Source<core::AccessUnitPayload>, public util::OriginalSource {
   private:
    util::BitReader reader;
    std::map<size_t, core::parameter::ParameterSet> parameterSets;
    util::OrderedLock lock;
    genie::core::stats::PerfStats *stats;
    mgb::DataUnitFactory factory;

   public:
    explicit Importer(std::istream& _file, genie::core::stats::PerfStats *_stats = nullptr);

    bool pump(size_t& id) override;

    /**
     * @brief Signal end of data.
     */
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

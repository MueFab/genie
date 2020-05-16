/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MGB_IMPORTER_H
#define GENIE_MGB_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-importer-compressed.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/ordered-section.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include <map>
#include "data-unit-factory.h"
#include "format-importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Importer : public core::FormatImporterCompressed {
   private:
    util::BitReader reader;
    std::map<size_t, core::parameter::ParameterSet> parameterSets;
    std::mutex lock;
    uint64_t pos_counter = 0;
    genie::core::stats::PerfStats* stats;
    mgb::DataUnitFactory factory;

   public:
    explicit Importer(std::istream& _file, genie::core::stats::PerfStats* _stats = nullptr);

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

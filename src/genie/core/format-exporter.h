/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_EXPORTER_H
#define GENIE_FORMAT_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit.h>
#include <genie/core/record/chunk.h>
#include <genie/util/drain.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for exporters of various file formats. Note that each importer has to convert from mpegg-records
 */
class FormatExporter : public util::Drain<record::Chunk> {
   private:
    stats::PerfStats stats;  //!<

   public:
    /**
     *
     * @return
     */
    stats::PerfStats& getStats();

    /**
     *
     * @param id
     */
    void skipIn(const util::Section& id) override;

    /**
     *
     */
    void flushIn(uint64_t& pos) override;

    /**
     *
     */
    ~FormatExporter() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
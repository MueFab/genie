/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FORMAT_EXPORTER_COMPRESSED_H
#define GENIE_FORMAT_EXPORTER_COMPRESSED_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/drain.h>
#include "access-unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FormatExporterCompressed : public util::Drain<AccessUnit> {
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
    void flushIn(size_t& pos) override;

    /**
     *
     */
    ~FormatExporterCompressed() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FORMAT_EXPORTER_COMPRESSED_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
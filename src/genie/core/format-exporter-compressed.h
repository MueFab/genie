/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_EXPORTER_COMPRESSED_H_
#define SRC_GENIE_CORE_FORMAT_EXPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/util/drain.h"

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
    void flushIn(uint64_t& pos) override;

    /**
     *
     */
    ~FormatExporterCompressed() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_EXPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

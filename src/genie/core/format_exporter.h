/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FORMAT_EXPORTER_H_
#define SRC_GENIE_CORE_FORMAT_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/record/chunk.h"
#include "genie/util/drain.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Interface for exporters of various file formats. Note that each importer has to convert from mpegg-records
 */
class FormatExporter : public util::Drain<record::Chunk> {
 private:
    stats::PerfStats stats;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    stats::PerfStats& getStats();

    /**
     * @brief
     * @param id
     */
    void SkipIn(const util::Section& id) override;

    /**
     * @brief
     */
    void FlushIn(uint64_t& pos) override;

    /**
     * @brief
     */
    ~FormatExporter() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

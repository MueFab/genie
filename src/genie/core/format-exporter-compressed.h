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

namespace genie::core {

/**
 * @brief
 */
class FormatExporterCompressed : public util::Drain<AccessUnit> {
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
    void skipIn(const util::Section& id) override;

    /**
     * @brief
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief
     */
    ~FormatExporterCompressed() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FORMAT_EXPORTER_COMPRESSED_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

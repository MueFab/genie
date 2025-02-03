/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_EXPORTER_H_
#define SRC_GENIE_FORMAT_MGB_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/format-exporter-compressed.h"
#include "genie/core/format-exporter.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-section.h"
#include "genie/util/ordered_lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class Exporter : public core::FormatExporterCompressed {
 private:
    util::BitWriter writer;                                      //!< @brief
    util::OrderedLock lock;                                      //!< @brief
    size_t id_ctr;                                               //!< @brief
    std::vector<core::parameter::ParameterSet> parameter_stash;  //!< @brief

 public:
    /**
     * @brief
     * @param _file
     */
    explicit Exporter(std::ostream* _file);

    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(core::AccessUnit&& t, const genie::util::Section& id) override;

    /**
     * @brief
     * @param id
     */
    void skipIn(const genie::util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

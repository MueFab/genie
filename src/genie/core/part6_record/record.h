/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_RECORD_VARIANT_SITE_H_
#define SRC_GENIE_CORE_RECORD_RECORD_VARIANT_SITE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/record/alignment/alignment-box.h"
#include "genie/core/record/alignment/alignment-external.h"
#include "genie/core/record/alignment/alignment-shared-data.h"
#include "genie/core/record/alignment/class-type.h"
#include "genie/core/record/alignment/segment.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_site {

/**
 *  @brief
 */
class Record {
 private:
    // TODO: To be filled

 public:
    /**
     * @brief
     */
    void patchRefID(size_t refID);

    /**
     * @brief
     */
    Record();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

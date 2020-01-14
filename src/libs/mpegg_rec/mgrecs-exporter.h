/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MGRECS_EXPORTER_H
#define GENIE_MGRECS_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitwriter.h>
#include <util/drain.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>
#include "mpegg-record.h"


namespace genie {
namespace mpegg_rec {
/**
 *
 */
class MgrecsExporter : public util::Drain<genie::mpegg_rec::MpeggChunk> {
    util::BitWriter writer;  //!<
    util::OrderedLock lock;  //!<

   public:
    /**
     *
     * @param _file_1
     */
    explicit MgrecsExporter(std::ostream& _file_1);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(genie::mpegg_rec::MpeggChunk&& t, size_t id) override;

    /**
     *
     */
    void dryIn() override;
};
}
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MGRECS_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
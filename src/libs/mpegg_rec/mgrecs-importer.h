/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MGRECS_IMPORTER_H
#define GENIE_MGRECS_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <backbone/mpegg_rec/alignment-container.h>
#include <backbone/mpegg_rec/external-alignment.h>
#include <backbone/mpegg_rec/meta-alignment.h>
#include <backbone/mpegg_rec/mpegg-record.h>
#include <backbone/mpegg_rec/segment.h>
#include <util/bitreader.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>
#include <util/original-source.h>
#include <util/source.h>

// ---------------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_rec {
class MgrecsImporter : public util::Source<genie::mpegg_rec::MpeggChunk>, public util::OriginalSource {
   private:
    size_t blockSize;
    util::BitReader reader;
    size_t record_counter;
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution

   public:
    MgrecsImporter(size_t _blockSize, std::istream &_file_1);

    bool pump(size_t id) override;

    void dryIn() override;
};
}
}
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MGRECS_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
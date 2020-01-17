/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_IMPORTER_H
#define GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/util/bitreader.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

class MgrecsImporter : public util::Source<core::record::MpeggChunk>, public util::OriginalSource {
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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
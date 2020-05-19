/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_IMPORTER_H
#define GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <classifier.h>
#include <format-importer.h>
#include <genie/core/record/chunk.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

class Importer : public core::FormatImporter {
   private:
    size_t blockSize;
    util::BitReader reader;
    size_t record_counter;

   public:
    Importer(size_t _blockSize, std::istream& _file_1);

    bool pumpRetrieve(core::Classifier* _classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

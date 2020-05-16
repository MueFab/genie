/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/util/ordered-section.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

MgrecsImporter::MgrecsImporter(size_t _blockSize, std::istream &_file_1, genie::core::stats::PerfStats *_stats)
    : blockSize(_blockSize), reader(_file_1), record_counter(0), stats(_stats) {}

// ---------------------------------------------------------------------------------------------------------------------

bool MgrecsImporter::pump(size_t &) {
    core::record::Chunk chunk;
    {
        //  util::OrderedSection section(&lock, id);
        for (size_t i = 0; i < blockSize; ++i) {
            core::record::Record rec(reader);
            if (!reader.isGood()) {
                //          flowOut(std::move(chunk), record_counter++);
                return false;
            }
            chunk.emplace_back(std::move(rec));
        }
    }
    //  flowOut(std::move(chunk), record_counter++);

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void MgrecsImporter::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

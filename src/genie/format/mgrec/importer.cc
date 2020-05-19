/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/util/ordered-section.h>
#include <genie/util/watch.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream& _file_1)
    : blockSize(_blockSize), reader(_file_1), record_counter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pumpRetrieve(core::Classifier* _classifier)  {
    util::Watch watch;
    core::record::Chunk chunk;
    {
        for (size_t i = 0; i < blockSize; ++i) {
            core::record::Record rec(reader);
            if (!reader.isGood()) {
                break;
            }
            chunk.getData().emplace_back(std::move(rec));
        }
    }

    chunk.getStats().addDouble("time-mgrec-import", watch.check());
    _classifier->add(std::move(chunk));
    return reader.isGood();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

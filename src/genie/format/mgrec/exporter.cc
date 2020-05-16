/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"
#include <genie/util/ordered-section.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

MgrecsExporter::MgrecsExporter(std::ostream &_file_1, genie::core::stats::PerfStats *_stats)
    : writer(&_file_1), stats(_stats) {}

// ---------------------------------------------------------------------------------------------------------------------

void MgrecsExporter::flowIn(core::record::Chunk &&t, const util::Section &id) {
    core::record::Chunk data = std::move(t);
    util::OrderedSection section(&lock, id);
    for (auto &i : data) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

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

MgrecsExporter::MgrecsExporter(std::ostream &_file_1) : writer(&_file_1) {}

// ---------------------------------------------------------------------------------------------------------------------

void MgrecsExporter::flowIn(core::record::MpeggChunk &&t, size_t id) {
    core::record::MpeggChunk data = std::move(t);
    util::OrderedSection section(&lock, id);
    for (auto &i : data) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MgrecsExporter::dryIn() {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgrec/exporter.h"
#include <utility>
#include "genie/util/ordered-section.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream &_file_1) : writer(_file_1) {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::record::Chunk &&t, const util::Section &id) {
    core::record::Chunk data = std::move(t);
    util::OrderedSection section(&lock, id);
    getStats().add(data.getStats());
    for (auto &i : data.getData()) {
        i.write(writer);
    }
}

void Exporter::skipIn(const util::Section &id) { util::OrderedSection sec(&lock, id); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(std::istream& _file) : reader(_file) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t id) {
    std::unique_ptr<mgb::AccessUnit> unit;
    {
        util::OrderedSection section(&lock, id);
        unit = factory.read(reader);
    }
    if (!unit) {
        return false;
    }
    core::AccessUnitPayload set(factory.getParams(unit->getParameterID()), id);

    for (auto& b : unit->getBlocks()) {
        set.setPayload(core::GenDesc(b.getDescriptorID()), b.movePayload());
    }
    set.setReference(unit->getAlignmentInfo().getRefID());
    set.setMinPos(unit->getAlignmentInfo().getStartPos());
    set.setMaxPos(unit->getAlignmentInfo().getEndPos());
    set.setRecordNum(unit->getReadCount());
    unit.reset();
    flowOut(std::move(set), id);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
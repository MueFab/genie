/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/util/watch.h>
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(std::istream& _file, genie::core::stats::PerfStats* _stats) : reader(_file), stats(_stats) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t& id, std::mutex&) {
    util::Watch watch;
    boost::optional<mgb::AccessUnit> unit;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> lock_guard(lock);
        unit = factory.read(reader);
        if (!unit) {
            return false;
        }
        sec.start = pos_counter;
        sec.length = unit->getReadCount();
        pos_counter += unit->getReadCount();
    }
    core::AccessUnitPayload set(factory.getParams(unit->getParameterID()), id);

    for (auto& b : unit->getBlocks()) {
        set.setPayload(core::GenDesc(b.getDescriptorID()), b.movePayload());
    }
    if (unit->getClass() != core::record::ClassType::CLASS_U) {
        set.setReference(unit->getAlignmentInfo().getRefID());
        set.setMinPos(unit->getAlignmentInfo().getStartPos());
        set.setMaxPos(unit->getAlignmentInfo().getEndPos());
    } else {
        set.setReference(0);
        set.setMinPos(0);
        set.setMaxPos(0);
    }
    set.setRecordNum(unit->getReadCount());
    set.setClassType(unit->getClass());
    unit.reset();
    set.getStats().addDouble("time-mgb-import", watch.check());
    flowOut(std::move(set), sec);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

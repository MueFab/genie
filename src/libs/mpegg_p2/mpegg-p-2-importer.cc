#include "mpegg-p-2-importer.h"
#include "access_unit.h"

namespace genie {
namespace mpegg_p2 {
MpeggP2Importer::MpeggP2Importer(std::istream& _file) : reader(_file) {}

bool MpeggP2Importer::pump(size_t id) {
    std::unique_ptr<mpegg_p2::AccessUnit> unit;
    {
        util::OrderedSection section(&lock, id);
        unit = factory.read(reader);
    }
    if (!unit) {
        return false;
    }
    genie::BlockPayloadSet set(factory.getParams(unit->getParameterID()), id);

    for (auto& b : unit->getBlocks()) {
        set.setPayload(genie::GenDesc(b.getDescriptorID()), b.movePayload());
    }
    set.setReference(unit->getAlignmentInfo().getRefID());
    set.setMinPos(unit->getAlignmentInfo().getStartPos());
    set.setMaxPos(unit->getAlignmentInfo().getEndPos());
    set.setRecordNum(unit->getReadCount());
    unit.reset();
    flowOut(std::move(set), id);
    return true;
}

/**
 * @brief Signal end of data.
 */
void MpeggP2Importer::dryIn() { dryOut(); }
}  // namespace mpegg_p2
}  // namespace genie
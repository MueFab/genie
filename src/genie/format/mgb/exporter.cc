/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"
#include <genie/util/watch.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream* _file) : writer(_file), id_ctr(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::AccessUnitPayload&& t, const util::Section& id) {
    util::Watch watch;
    core::AccessUnitPayload data = std::move(t);
    util::OrderedSection section(&lock, id);
    getStats().add(data.getStats());
    data.getParameters().setID(id_ctr);
    data.getParameters().setParentID(id_ctr);
    data.getParameters().write(writer);

    mgb::AccessUnit au(id_ctr, id_ctr, data.getClassType(), data.getRecordNum() * data.getParameters().getNumberTemplateSegments(),
                       data.getClassType() == core::record::ClassType::CLASS_U ? core::parameter::DataUnit::DatasetType::NON_ALIGNED : core::parameter::DataUnit::DatasetType::ALIGNED, 32, 32, 0);
    if(au.getClass() != core::record::ClassType::CLASS_U) {
        au.setAuTypeCfg(
            AuTypeCfg(id_ctr, data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
    }
    for (size_t descriptor = 0; descriptor < core::getDescriptors().size(); ++descriptor) {
        if (data.getPayload(core::GenDesc(descriptor)).isEmpty()) {
            continue;
        }
        au.addBlock(Block(descriptor, data.movePayload(descriptor)));
    }
    au.write(writer);
    id_ctr++;
    getStats().addDouble("time-mgb-export", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

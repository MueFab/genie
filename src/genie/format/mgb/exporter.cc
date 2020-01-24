/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream* _file) : writer(_file) {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::AccessUnitPayload&& t, size_t id) {
    core::AccessUnitPayload data = std::move(t);
    util::OrderedSection section(&lock, id);
    data.getParameters().write(writer);

    mgb::AccessUnit au(id, id, core::record::ClassType::CLASS_I, data.getRecordNum(),
                       core::parameter::DataUnit::DatasetType::ALIGNED, 32, 32, 0);
    au.setAuTypeCfg(
        AuTypeCfg(data.getReference(), data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
    for (size_t descriptor = 0; descriptor < core::getDescriptors().size(); ++descriptor) {
        if (data.getPayload(core::GenDesc(descriptor)).isEmpty()) {
            continue;
        }
        au.addBlock(Block(descriptor, data.movePayload(descriptor)));
    }
    au.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::dryIn() {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
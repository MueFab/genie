/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "exporter.h"
#include <genie/util/watch.h>
#include "raw_reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream* _file) : writer(_file), id_ctr(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::AccessUnit&& t, const util::Section& id) {
    util::Watch watch;
    core::AccessUnit data = std::move(t);
    util::OrderedSection section(&lock, id);
    getStats().add(data.getStats());
    data.getParameters().setID(id_ctr);
    data.getParameters().setParentID(id_ctr);
    mgb::RawReference ref;
    for(const auto& p : data.getRefToWrite()) {
        auto string = *data.getReferenceExcerpt().getChunkAt(p.first);
        mgb::RawReferenceSequence refseq(data.getReference(), p.first, std::move(string));
        ref.addSequence(std::move(refseq));
    }
    if(!ref.isEmpty()) {
        ref.write(writer);
        ref = mgb::RawReference();
    }

    if(data.getNumReads() == 0) {
        return;
    }

    data.getParameters().write(writer);

    auto datasetType = data.getClassType() != core::record::ClassType::CLASS_U
              ? core::parameter::DataUnit::DatasetType::ALIGNED : (data.isReferenceOnly() ? core::parameter::DataUnit::DatasetType::REFERENCE : core::parameter::DataUnit::DatasetType::NON_ALIGNED);

    mgb::AccessUnit au(id_ctr, id_ctr, data.getClassType(), data.getNumReads(),
                       datasetType,
                       32, 32, 0);
    if(data.isReferenceOnly()) {
        au.setRefCfg(RefCfg(data.getReference(), data.getReferenceExcerpt().getDataStart(), data.getReferenceExcerpt().getDataEnd() - 1, 32));
    }
    if (au.getClass() != core::record::ClassType::CLASS_U) {
        au.setAuTypeCfg(AuTypeCfg(data.getReference(), data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
    }
    for (size_t descriptor = 0; descriptor < core::getDescriptors().size(); ++descriptor) {
        if (data.get(core::GenDesc(descriptor)).isEmpty()) {
            continue;
        }
        au.addBlock(Block(descriptor, std::move(data.get(core::GenDesc(descriptor)))));
    }
    au.write(writer);
    id_ctr++;
    getStats().addDouble("time-mgb-export", watch.check());
}

void Exporter::skipIn(const genie::util::Section& id) { util::OrderedSection sec(&lock, id); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
